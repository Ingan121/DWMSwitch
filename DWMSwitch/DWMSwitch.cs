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

            Process[] processes = Process.GetProcessesByName("dwm");
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
                    Process[] processes = Process.GetProcessesByName("dwm");
                    if (processes.Length > 0)
                    {
                        processes[0].Kill();
                        notifyIcon.Icon = Properties.Resources.DWMOff;
                    }
                    else
                    {
                        Process.Start("dwm.exe start");
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
    }
}
