namespace UNET.Exceptions;

public class NotInitializedException : UNETException
{
    public NotInitializedException()
    {
    }

    public NotInitializedException(string message) : base("UNET is not initialized yet")
    {
    }

    public NotInitializedException(string message, Exception innerException) : base(message, innerException)
    {
    }
}