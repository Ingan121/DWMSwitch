using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;
using System.Threading;

namespace DWMSwitch
{
    public partial class SockTest : Form
    {
        public SockTest()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
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
                MessageBox.Show("Server Start");
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
                        MessageBox.Show("Client connected IP address = " + ip.Address + " : " + ip.Port);
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
                                    client.Send(Encoding.Unicode.GetBytes("echo - " + msg + ">\0"), SocketFlags.None);
                                    // 콘솔 출력	
                                    MessageBox.Show(msg);
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
                        MessageBox.Show("Client disconnected IP address = " + ip.Address + " : " + ip.Port);
                        // 클라이언트와 접속이 되면 Thread 생성	
                    }, server.Accept());
                }
                server.Close();
            });
        }
    }
}
