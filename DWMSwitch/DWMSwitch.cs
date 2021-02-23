using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Reflection;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace DWMSwitch
{
    public partial class DWMSwitch : Form
    {
        #region Variables

        // Global
        public string logs;
        public string[] args = Environment.GetCommandLineArgs();
        //public RegistryKey dsConfReg = Registry.LocalMachine.CreateSubKey("SOFTWARE\\Ingan121\\DWMSwitch", RegistryKeyPermissionCheck.ReadWriteSubTree);
        public Version ver = new Version(FileVersionInfo.GetVersionInfo(Assembly.GetExecutingAssembly().Location).FileVersion);
        public bool donthide = false;

        #endregion

        #region Main

        public DWMSwitch()
        {
            InitializeComponent();

            if (args.Any(x => x.Contains("hidetray")))
            {
                notifyIcon.Visible = false;
            }

            if (!args.Any(x => x.Contains("nosock")))
            {
                StartSockServer();
            }

            if (!args.Any(x => x.Contains("noupdchk")))
            {
                updateCheck();
            }
        }

        #endregion

        private void DWMSwitch_Shown(object sender, EventArgs e)
        {
            // Wait until Explorer starts to ensure that the tray icon appears
            while (Process.GetProcessesByName("explorer").Length == 0)
            {
                Thread.Sleep(100);
            }

            EnableWow64FSRedirection(false);

            Process[] processes = Process.GetProcessesByName("dwm.orig");
            if (processes.Length > 0)
            {
                notifyIcon.Icon = Properties.Resources.DWMOn;
            }
            else
            {
                notifyIcon.Icon = Properties.Resources.DWMOff;
            }

            notifyIcon.Visible = true;
            Visible = false;
        }

        private void notifyIcon_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                try
                {
                    Process[] processes = Process.GetProcessesByName("dwm.orig");
                    if (processes.Length > 0)
                    {
                        processes[0].Kill();
                        notifyIcon.Icon = Properties.Resources.DWMOff;
                    }
                    else
                    {
                        Process.Start("dwm.orig.exe");
                        notifyIcon.Icon = Properties.Resources.DWMOn;
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.ToString(), "DWMSwitch Error");
                }
            }
        }

        [System.Runtime.InteropServices.DllImport("Kernel32.Dll", EntryPoint = "Wow64EnableWow64FsRedirection")]
        public static extern bool EnableWow64FSRedirection(bool enable);

        private void launchCMDAsDWMUserToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Process.Start("cmd.exe");
        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show(Resources.Strings.ExitConfirmMessage, Resources.Strings.AppName, MessageBoxButtons.YesNo) == DialogResult.Yes)
            {
                Application.Exit();
            }
        }

        private void gitHubToolStripMenuItem_Click(object sender, EventArgs e) => Process.Start("https://github.com/Ingan121/DWMSwitch/releases");

        private void checkForUpdatesToolStripMenuItem_Click(object sender, EventArgs e)
        {
            updateCheck(true);
        }

        public void log(string str, bool alwaysLog = false) // Logs currently not visible
        {
            logs += string.Format("\n{0} : " + str, DateTime.Now);
        }

        private void updateCheck(bool alertLatest = false)
        {
            Task.Factory.StartNew(() =>
            {
                log("[Checking for updates...]", true);
                try
                {
                    WebClient wc = new WebClient();
                    string latestVerStr = wc.DownloadString("https://raw.githubusercontent.com/Ingan121/DWMSwitch/master/latest.txt");
                    //string latestVerStr = wc.DownloadString("http://localhost/latest.txt");
                    log("[Lastest version found: " + latestVerStr + "]", true);

                    Version latestVer = new Version(latestVerStr);
                    int compare = ver.CompareTo(latestVer);

                    if (compare < 0)
                    {
                        if (MessageBox.Show("New version of DWMSwitch is available. Download it now?", "DWMSwitch", MessageBoxButtons.YesNo) == DialogResult.Yes)
                        {
                            Process.Start("https://github.com/Ingan121/BasicThemer2/releases");
                        }
                    }
                    else
                    {
                        if (alertLatest)
                        {
                            if (compare == 0)
                            {
                                MessageBox.Show("You are running the latest version of DWMSwitch.", "DWMSwitch");
                            }
                            else
                            {
                                MessageBox.Show("You are running a unreleased version of DWMSwitch.", "DWMSwitch");
                            }
                        }
                    }
                }
                catch (Exception ex)
                {
                    log(ex.ToString(), true);
                    MessageBox.Show("Update check failed!", "DWMSwitch");
                }
            });
        }

        private void settingsToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show("Work in Progress");
            //new SockTest().Show();
        }

        private void StartSockServer()
        {
            Task.Factory.StartNew(() =>
            {
                // 포트 9090으로 서버 설정	
                IPEndPoint ipep = new IPEndPoint(IPAddress.Any, 9090);
                Socket server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                // bind하고 서버 Listen 대기	
                server.Bind(ipep);
                server.Listen(20);
                // 콘솔 출력	
                log("Server Start");
                // 다중 접속 시작	
                while (true)
                {
                    // ThreadPool로 접속이 되면	
                    ThreadPool.QueueUserWorkItem((_) =>
                    {
                        // 메시지 버퍼	
                        StringBuilder sb = new StringBuilder();
                        // 클라이언트 받기	
                        Socket client = (Socket)_;
                        // 클라이언트 정보	
                        IPEndPoint ip = (IPEndPoint)client.RemoteEndPoint;
                        // 콘솔 출력	
                        log("Client connected IP address = " + ip.Address + " : " + ip.Port);
                        // 메시지 전송	
                        client.Send(Encoding.Unicode.GetBytes("Welcome server!\r\n>\0"), SocketFlags.None);
                        try
                        {
                            // 수신 대기	
                            while (true)
                            {
                                // 서버로 오는 메시지를 받는다.	
                                byte[] ret = new byte[2];
                                client.Receive(ret, 2, SocketFlags.None);
                                // 메시지를 unicode로 변환해서 버퍼에 넣는다.	
                                sb.Append(Encoding.Unicode.GetString(ret, 0, 2));
                                // 개행 + \n이면 콘솔 출력한다.	
                                if (sb.Length >= 2 && sb[sb.Length - 2] == '\r' && sb[sb.Length - 1] == '\n')
                                {
                                    // exit면 접속을 끊는다.	
                                    if (sb.Length >= 4 && sb[sb.Length - 4] == 'e' && sb[sb.Length - 3] == 'x' && sb[sb.Length - 2] == 'i' && sb[sb.Length - 1] == 't')
                                    {
                                        break;
                                    }
                                    // 버퍼의 메시지를 콘솔에 출력	
                                    string msg = sb.ToString();
                                    // echo 메시지 전송	
                                    //client.Send(Encoding.Unicode.GetBytes("echo - " + msg + ">\0"), SocketFlags.None);
                                    // 콘솔 출력	
                                    log(msg);

                                    try
                                    {
                                        Process[] processes = Process.GetProcessesByName("dwm.orig");
                                        if (processes.Length > 0)
                                        {
                                            processes[0].Kill();
                                            notifyIcon.Icon = Properties.Resources.DWMOff;
                                        }
                                        else
                                        {
                                            Process.Start("C:\\Windows\\Sysnative\\dwm.orig.exe");
                                            notifyIcon.Icon = Properties.Resources.DWMOn;
                                        }
                                    }
                                    catch (Exception ex)
                                    {
                                        client.Send(Encoding.Unicode.GetBytes("Error!" + ex.ToString() + "\r\n>\0"), SocketFlags.None);
                                        MessageBox.Show(ex.ToString());
                                    }

                                    // 버퍼를 비운다.	
                                    sb.Clear();
                                }
                            }
                        }
                        catch
                        {
                            // 에러 발생하면 종료	
                        }
                        // 접속 종료 메시지 콘솔 출력	
                        log("Client disconnected IP address = " + ip.Address + " : " + ip.Port);
                        // 클라이언트와 접속이 되면 Thread 생성	
                    }, server.Accept());
                }
                server.Close();
            });
        }
    }
}
