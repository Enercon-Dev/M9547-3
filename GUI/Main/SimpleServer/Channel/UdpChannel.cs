using System;
using System.Net.Sockets;
using System.Net;


namespace SimpleServer.Channel
{
    class UdpChannel : ITransferChannel
    {
        public event SimpleServerMessageHandler OnMessageReceived;
        public event SimpleServerChannelErrorHandler OnError;

        private IPEndPoint endpoint;
        private UdpClient client;
        //private readonly CommandSerializer serializer = new CommandSerializer();
        //private readonly CommandParser parser = new CommandParser();

        public UdpChannel(UdpClient client)
        {
            this.client = client;
            this.endpoint = null;
        }

        public void Open()
        {
            if (client == null)
                throw new ObjectDisposedException("UdpChannel");

            client.BeginReceive(UdpReceivedCallback, null);
        }

        void UdpReceivedCallback(IAsyncResult result)
        {
            IPEndPoint endpoint = new IPEndPoint(0, 0);
            //ITftpCommand command = null;
            byte[] data = null;
            try
            {
                

                lock (this)
                {
                    if (client == null)
                        return;

                    data = client.EndReceive(result, ref endpoint);
                }
                //command = parser.Parse(data);
            }
            catch (SocketException e)
            {
                //Handle receive error
                RaiseOnError(new NetworkError(e));
            }
            
            if (data != null)
                OnMessageReceived(data, endpoint);

            lock (this)
            {
                if (client != null)
                    client.BeginReceive(UdpReceivedCallback, null);
            }
        }


        private void RaiseOnError(SimpleServerTransferError error)
        {
            if (OnError != null)
                OnError(error);
        }

        public void Send(byte[] message)
        {
            if (client == null)
                throw new ObjectDisposedException("UdpChannel");

            if (endpoint == null)
                throw new InvalidOperationException("RemoteEndpoint needs to be set before you can send TFTP commands.");

            client.Send(message, message.Length, endpoint);
        }

        public void Dispose()
        {
            lock (this)
            {
                if (this.client == null)
                    return;

                client.Close();
                this.client = null;
            }
        }

        public EndPoint RemoteEndpoint
        {
            get
            {
                return endpoint;
            }

            set
            {
                if (!(value is IPEndPoint))
                    throw new NotSupportedException("UdpChannel can only connect to IPEndPoints.");

                if (client == null)
                    throw new ObjectDisposedException("UdpChannel");

                this.endpoint = (IPEndPoint)value;
            }
        }
    }
}
