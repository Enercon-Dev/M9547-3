using System;

namespace SimpleServer
{
    /// <summary>
    /// Base class for all errors that may be passed to <code>ITftpTransfer.OnError</code>.
    /// </summary>
    public abstract class SimpleServerTransferError
    {
        public abstract override String ToString();
    }

    /// <summary>
    /// Errors that are sent from the remote party using the TFTP Error Packet are represented
    /// by this class.
    /// </summary>
    public class FilterErrorPacket : SimpleServerTransferError
    {
        /// <summary>
        /// Error code that was sent from the other party.
        /// </summary>
        public ushort ErrorCode { get; private set; }

        /// <summary>
        /// Error description that was sent by the other party.
        /// </summary>
        public string ErrorMessage { get; private set; }

        public FilterErrorPacket(ushort errorCode, string errorMessage)
        {
            if (String.IsNullOrEmpty(errorMessage))
                throw new ArgumentException("You must provide an errorMessage.");

            this.ErrorCode = errorCode;
            this.ErrorMessage = errorMessage;
        }

        public override string ToString()
        {
            return ErrorCode + " - " + ErrorMessage;
        }

        #region Predefined error packets from RFC 1350
        public static readonly FilterErrorPacket EmptyFrame = new FilterErrorPacket(1, "Data Not Recieved");
        
        #endregion
    }

    /// <summary>
    /// Network errors (i.e. socket exceptions) are represented by this class.
    /// </summary>
    public class NetworkError : SimpleServerTransferError
    {
        public Exception Exception { get; private set; }

        public NetworkError(Exception exception)
        {
            this.Exception = exception;
        }

        public override string ToString()
        {
            return Exception.ToString();
        }
    }

    /// <summary>
    /// $(ITftpTransfer.RetryTimeout) has been exceeded more than $(ITftpTransfer.RetryCount) times in a row.
    /// </summary>
    public class TimeoutError : SimpleServerTransferError
    {
        private readonly TimeSpan RetryTimeout;
        private readonly int RetryCount;

        public TimeoutError(TimeSpan retryTimeout, int retryCount)
        {
            this.RetryTimeout = retryTimeout;
            this.RetryCount = retryCount;
        }

        public override string ToString()
        {
            return "Timeout error. RetryTimeout (" + RetryCount + ") violated more than " + RetryCount + " times in a row";
        }
    }
}
