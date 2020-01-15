using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Threading;

enum CLIENTS
{
   PORT =44000
}

namespace TcpClientServer
{
    public class cServer
    {        
        private int ClientNbr =0;
        public static string serverData =null;

        TcpListener serverSocket;

        // start a server socket, and start a thread
        // that listens for client connections
        public void StartServerSocket()
        {
           string theServer = Dns.GetHostName();
           IPHostEntry hostInfo = Dns.GetHostEntry(theServer);

           serverSocket = new TcpListener(hostInfo.AddressList[0], (int)CLIENTS.PORT);
           serverSocket.Start();

           Thread listenThread = new Thread(clientListener);
           listenThread.Start();
        }

        // listen for client connections
        // create a client handler for any new connections
        private void clientListener() 
        {
            TcpClient clientSocket = default(TcpClient);
            while(true)
            {
               clientSocket = serverSocket.AcceptTcpClient();
               ClientNbr++;

               serverData = "Client#" + ClientNbr.ToString() + " Accepted";

               handleClient client = new handleClient();
               client.startClient(clientSocket, Convert.ToString(ClientNbr));
            }
        }
    }


    //Class for the server to handle each client request separately      
    public class handleClient
    {
        TcpClient clientSocket;
        string clNo;
        public static string dataFromClient = null;

        public void startClient(TcpClient inClientSocket, string clineNo)
        {
           this.clientSocket = inClientSocket;
           this.clNo = clineNo;

           Thread ctThread = new Thread(doChat);
           ctThread.Start();
        }

        // threaded routine that monitors packets from the client
        // server responds to client when a packet has been received
        // client disconnects are also caught here (catch (Exception ex))
        private void doChat()
        {
            int requestCount = 0;
            byte[] bytesFrom = new byte[10025];
            byte[] sendBytes = null;
            string serverResponse = null;
            string rCount = null;

            requestCount = 0;

            while(true)
            {
                try
                {
                    requestCount = requestCount + 1;
                    NetworkStream networkStream = clientSocket.GetStream();
                    networkStream.Read(bytesFrom, 0, (int)clientSocket.ReceiveBufferSize);
                    dataFromClient = System.Text.Encoding.ASCII.GetString(bytesFrom);

                    rCount = Convert.ToString(requestCount);
                    serverResponse = "Server to client(" + clNo + ") " + rCount +'\n';
                    sendBytes = Encoding.ASCII.GetBytes(serverResponse);
                    networkStream.Write(sendBytes, 0, sendBytes.Length);
                    networkStream.Flush();
                }
                catch (Exception ex)
                {
                    clientSocket.Close();
                    System.GC.Collect();
                    dataFromClient = "Client: " + clNo.ToString() +" " + ex.ToString(); ;
                    // stop this thread
                    return;
                }
            }
        }
    }

    /******** CLIENT SOCKET CLASS ********/

    // Class to initiate a client connection
    public class cClient
    {
        private int MsgCount = 0;

        private TcpClient clientSocket = new TcpClient();

        public String StartClient(string ipAddr, int port)
        {
            // 127.0.0.1 loop back
            // 192.168.1.109 Jeff's PC IP
            // server is "Jeff"
            // clientSocket.Connect( ) can expect either host name of "Jeff" or
            // the ip address heserver.AddressList[0], this is resolved from  
            // IPHostEntry hostInfo = Dns.GetHostEntry(server);
            // IPHostEntry hostInfo = Dns.GetHostEntry(server);
            try
            {
                clientSocket.Connect(ipAddr, port);//ipAddr, port); //(hostInfo.AddressList[0], port);               
                return "Client connect to " + ipAddr + '\x0'; //successful";
            }
            catch (Exception ex)
            {
                System.GC.Collect();                
                return ex.ToString();
            }
        }

        // send packets to server
        // receive packets form server
        public String SocketSendReceive()
        {
            MsgCount++;

            try
            {
                NetworkStream serverStream = clientSocket.GetStream();
                byte[] outStream = System.Text.Encoding.ASCII.GetBytes("Message from Client " + " Msg #:" + MsgCount.ToString());
                serverStream.Write(outStream, 0, outStream.Length);
                serverStream.Flush();

                byte[] inStream = new byte[10025];
                serverStream.Read(inStream, 0, (int)clientSocket.ReceiveBufferSize);
                string returndata = System.Text.Encoding.ASCII.GetString(inStream);

                return returndata;
            }
            catch (Exception ex)
            {
                return ex.ToString();
            }
        }
    }
}






