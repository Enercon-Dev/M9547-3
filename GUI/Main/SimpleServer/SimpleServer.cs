using SimpleServer.Channel;
using System;
using System.Net;
using System.Text;
using System.Threading;
using System.Timers;

namespace SimpleServer
{
    public delegate void SimpleServerEventHandler(byte[] Message, EndPoint client);
    public delegate void SimpleServerErrorHandler(SimpleServerTransferError error);

    /// <summary>
    /// A simple TFTP server class. <code>Dispose()</code> the server to close the socket that it listens on.
    /// </summary>
    public class TSimpleServer : IDisposable
    {
        public const int DEFAULT_COMMAND_PORT = 5000;
        public const int DEFAULT_KEPPAVLIVE_PORT = 5010;
        public const int DEFAULT_ERUPT_PORT = 5020;
        System.Timers.Timer tim = new System.Timers.Timer(10000);

        /// <summary>
        /// Fired when the server receives an Eruptive message.
        /// </summary>
        public event SimpleServerEventHandler OnErupt;

        /// <summary>
        /// Fired when the server receives a Response of Command.
        /// </summary>
        public event SimpleServerEventHandler OnResponse;

        /// <summary>
        /// Fired when the server encounters an error (for example, a non-parseable request)
        /// </summary>
        public event SimpleServerErrorHandler OnError;

        /// <summary>
        /// Server ports that we're listening on.
        /// </summary>
        private readonly ITransferChannel serverResponseSocket;
        private readonly ITransferChannel serverKeepAliveSocket;
        private readonly ITransferChannel serverEruptSocket;

        static private SynchronizationContext MainThread;

        public TSimpleServer(IPEndPoint ResponseAddress, IPEndPoint KeepAliveAddress, IPEndPoint EruptAddress)
        {
            if (ResponseAddress == null)
                throw new ArgumentNullException("Response Address");
            if ( EruptAddress  == null)
                throw new ArgumentNullException("Erupt Address");
            if (KeepAliveAddress == null)
                throw new ArgumentNullException("KeepAlive Address");

            MainThread = SynchronizationContext.Current;

            serverResponseSocket = TransferChannelFactory.CreateServer(ResponseAddress);
            serverResponseSocket.OnMessageReceived += new SimpleServerMessageHandler(serverResponseSocket_OnMessageReceived);
            serverResponseSocket.OnError += new SimpleServerChannelErrorHandler(serverSocket_OnError);
            serverKeepAliveSocket = TransferChannelFactory.CreateServer(KeepAliveAddress);
            serverKeepAliveSocket.OnMessageReceived += new SimpleServerMessageHandler(serverEruptSocket_OnMessageReceived);
            serverKeepAliveSocket.OnError += new SimpleServerChannelErrorHandler(serverSocket_OnError);
            serverEruptSocket = TransferChannelFactory.CreateServer(EruptAddress);
            serverEruptSocket.OnMessageReceived += new SimpleServerMessageHandler(serverEruptSocket_OnMessageReceived);
            serverEruptSocket.OnError += new SimpleServerChannelErrorHandler(serverSocket_OnError);
            tim.AutoReset = true;
            tim.Elapsed += Tim_Elapsed;
        }

        private void Tim_Elapsed(object sender, ElapsedEventArgs e)
        {
            serverKeepAliveSocket.RemoteEndpoint = new IPEndPoint(IPAddress.Broadcast, 5050);
            serverKeepAliveSocket.Send(Encoding.ASCII.GetBytes("Hello!!"));
            serverEruptSocket.RemoteEndpoint = new IPEndPoint(IPAddress.Broadcast, 5050);
            serverEruptSocket.Send(Encoding.ASCII.GetBytes("Hello!!"));
        }
        public TSimpleServer(IPAddress localAddress)
            : this(localAddress, DEFAULT_COMMAND_PORT, DEFAULT_KEPPAVLIVE_PORT, DEFAULT_ERUPT_PORT)
        {
        }

        public TSimpleServer(IPAddress localAddress, int Response_port, int KeepAlive_port, int Erupt_port)
            : this(new IPEndPoint(localAddress, Response_port),new IPEndPoint(localAddress,KeepAlive_port), new IPEndPoint(localAddress, Erupt_port))
        {
        }

        public TSimpleServer(int Response_port, int KeepAlive_port, int Erupt_port)
            : this(new IPEndPoint(IPAddress.Any, Response_port), new IPEndPoint(IPAddress.Any, KeepAlive_port), new IPEndPoint(IPAddress.Any, Erupt_port))
        {
        }

        public TSimpleServer()
            : this(DEFAULT_COMMAND_PORT, DEFAULT_KEPPAVLIVE_PORT , DEFAULT_ERUPT_PORT)
        {
        }


        /// <summary>
        /// Start accepting incoming connections.
        /// </summary>
        public void Start()
        {
            serverResponseSocket.Open();
            serverKeepAliveSocket.Open();
            serverEruptSocket.Open();
        }

        public void Send(byte[] message, EndPoint endpoint)
        {
            serverResponseSocket.RemoteEndpoint = endpoint;
            serverResponseSocket.Send(message);
            /*
            using (ITransferChannel SendSocket = TransferChannelFactory.CreateConnection(endpoint))
            {
                SendSocket.Send(message);
            }
            */
        }
        public void Send(byte[] message, IPAddress ipaddress)
        {
            Send(message, new IPEndPoint(ipaddress, DEFAULT_COMMAND_PORT));
        }

        void serverSocket_OnError(SimpleServerTransferError error)
        {
            if (OnError != null)
                OnError(error);
        }

        private void serverResponseSocket_OnMessageReceived(byte[] message, EndPoint endpoint)
        {
            if (message == null || message.Length < 4)
                RaiseOnError(FilterErrorPacket.EmptyFrame);
            if (endpoint == null)
                throw new Exception("No IP source recieved");
            RaiseResponseRecieve(message, endpoint);
        }

        private void serverEruptSocket_OnMessageReceived(byte[] message, EndPoint endpoint)
        {
            if (message == null || message.Length < 4)
                RaiseOnError(FilterErrorPacket.EmptyFrame);
            if (endpoint == null)
                throw new Exception("No IP source recieved");
            MainThread.Send(o => {
                OnErupt(message, endpoint);
            }, null);
        }

        #region IDisposable
        public void Dispose()
        {
            serverEruptSocket.Dispose();
            serverResponseSocket.Dispose();
        }
        #endregion

        private void RaiseOnError(SimpleServerTransferError error)
        {
            if (OnError != null)
                MainThread.Send(o => {
                    OnError(error);
                }, null);
            
        }

        private void RaiseResponseRecieve(byte[] message, EndPoint client)
        {
            if (OnResponse != null)
            {
                
                    MainThread.Send(o => {
                        OnResponse(message, client);
                    }, null); 
            }
        }

        private void RaiseEruptRecieve(byte[] message, EndPoint client)
        {
            if (OnErupt != null)
            {
                MainThread.Send(o => {
                    OnErupt(message, client);
                }, null); 
            }
        }
    }
}

