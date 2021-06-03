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
        public bool allowShowDisplay = false;
        public bool dontHide = false;


        #endregion

        #region Main

        public DWMSwitch()
        {
            InitializeComponent();

            if (args.Any(x => x.Contains("hidetray")))
            {
                notifyIcon.Visible = false;
            }

            if (!args.Any(x => x.Contains("noupdchk")))
            {
                updateCheck();
            }

            EnableWow64FSRedirection(false);

            Process[] processes = Process.GetProcessesByName("dwm");
            if (processes.Length > 0)
            {
                notifyIcon.Icon = Properties.Resources.DWMOn;
            }
            else
            {
                notifyIcon.Icon = Properties.Resources.DWMOff;
            }
        }
        #endregion

        private void notifyIcon_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                try
                {
                    Process[] processes = Process.GetProcessesByName("dwm");
                    if (processes.Length > 0)
                    {
                        //sendMsgToServer("taskkill -im dwm.exe -f");
                        ProcessStartInfo info = new ProcessStartInfo("C:\\Windows\\dwmctl.exe");
                        info.Arguments = "stop";
                        Process.Start(info);
                        notifyIcon.Icon = Properties.Resources.DWMOff;
                    }
                    else
                    {
                        //sendMsgToServer("start C:\\Windows\\System32\\DWM\\dwm.exe");
                        ProcessStartInfo info = new ProcessStartInfo("C:\\Windows\\dwmctl.exe");
                        info.Arguments = "start";
                        Process.Start(info);
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

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Application.Exit();
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

        private void sendMsgToServer(string msg)
        {
            // https://nowonbun.tistory.com/737
            // 개행 코드
            char CR = (char)0x0D;
            char LF = (char)0x0A;
            // 수신 버퍼
            StringBuilder sb = new StringBuilder();
            // 소켓을 연다.
            using (Socket socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.IP))
            {
                // 로컬의 9090포트로 접속한다.
                socket.Connect(IPAddress.Parse("127.0.0.1"), 35489);
                // 수신을 위한 쓰레드
                ThreadPool.QueueUserWorkItem((_) =>
                {
                    while (true)
                    {
                        // 서버로 오는 메시지를 받는다.
                        byte[] ret = new byte[2];
                        socket.Receive(ret, 2, SocketFlags.None);
                        // 메시지를 unicode로 변환해서 버퍼에 넣는다.
                        sb.Append(Encoding.Unicode.GetString(ret, 0, 2));
                        // 개행 + \n이면 콘솔 출력한다.
                        if (sb.Length >= 4 && sb[sb.Length - 4] == CR && sb[sb.Length - 3] == LF && sb[sb.Length - 2] == '>' && sb[sb.Length - 1] == '\0')
                        {
                            // 버퍼의 메시지를 로그에 출력
                            log(sb.ToString());
                            // 버퍼를 비운다.
                            sb.Clear();
                        }
                    }
                });
                // 송신을 위한 입력대기
                byte[] data = Encoding.Unicode.GetBytes(msg + "\r\n");
                // 송신.
                socket.Send(data, data.Length, SocketFlags.None);
            }
        }

        protected override void SetVisibleCore(bool value)
        {
            if (args.Any(x => x.Contains("showui")))
            {
                allowShowDisplay = true;
            }
            if (args.Any(x => x.Contains("dontHide")))
            {
                dontHide = true;
            }

            base.SetVisibleCore(allowShowDisplay ? value : allowShowDisplay);
        }
    }
}
