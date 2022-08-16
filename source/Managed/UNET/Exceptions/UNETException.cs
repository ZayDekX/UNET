namespace UNET.Exceptions;

public abstract class UNETException : Exception
{
    protected UNETException()
    {
    }

    protected UNETException(string message) : base(message)
    {
    }

    protected UNETException(string message, Exception innerException) : base(message, innerException)
    {
    }
}
