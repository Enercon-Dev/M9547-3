using System;
using System.Net;
using System.Net.Sockets;
using System.Text;

namespace SimpleServer.Channel
{
    static class TransferChannelFactory
    {
        public static ITransferChannel CreateServer(EndPoint localAddress)
        {
            if (localAddress is IPEndPoint)
                return CreateServerUdp((IPEndPoint)localAddress);

            throw new NotSupportedException("Unsupported endpoint type.");
        }

        public static ITransferChannel CreateConnection(EndPoint remoteAddress)
        {
            if (remoteAddress is IPEndPoint)
                return CreateConnectionUdp((IPEndPoint)remoteAddress);

            throw new NotSupportedException("Unsupported endpoint type.");
        }

        #region UDP connections

        private static ITransferChannel CreateServerUdp(IPEndPoint localAddress)
        {
            try
            {
                UdpClient socket = new UdpClient(localAddress);
                socket.EnableBroadcast = true;
                socket.Send(Encoding.ASCII.GetBytes("Hello!!"), 7, "255.255.255.255", 5049);
                return new UdpChannel(socket);
            }
            catch (SocketException e)
            {
                System.Windows.Forms.MessageBox.Show(e.Message);
                throw e;
            }
            
        }

        private static ITransferChannel CreateConnectionUdp(IPEndPoint remoteAddress)
        {
            IPEndPoint localAddress = new IPEndPoint(IPAddress.Any, 0);
            UdpChannel channel = new UdpChannel(new UdpClient(localAddress));
            channel.RemoteEndpoint = remoteAddress;
            return channel;
        }
        #endregion
    }
}
