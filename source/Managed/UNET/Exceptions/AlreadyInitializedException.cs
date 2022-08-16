namespace UNET.Exceptions;

public class AlreadyInitializedException : UNETException
{
    public AlreadyInitializedException()
    {
    }

    public AlreadyInitializedException(string message) : base("UNET is already initialized")
    {
    }

    public AlreadyInitializedException(string message, Exception innerException) : base(message, innerException)
    {
    }
}