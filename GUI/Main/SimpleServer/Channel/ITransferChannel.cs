using System;
using System.Net;

namespace SimpleServer.Channel
{
    delegate void SimpleServerMessageHandler(byte[] message, EndPoint endpoint);
    delegate void SimpleServerChannelErrorHandler(SimpleServerTransferError error);

    interface ITransferChannel : IDisposable
    {
        event SimpleServerMessageHandler OnMessageReceived;
        event SimpleServerChannelErrorHandler OnError;

        EndPoint RemoteEndpoint { get; set; }

        void Open();
        void Send(byte[] buffer);
    }
}
