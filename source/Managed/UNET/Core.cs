using UNET.Exceptions;

namespace UNET;

public static unsafe class Core
{
    public static bool IsInitialized { get; private set; }

    private static NativeDelegates _nativeDelegates;

    internal static ref NativeDelegates NativeDelegates
    {
        get
        {
            if (!IsInitialized)
            {
                throw new NotInitializedException();
            }

            return ref _nativeDelegates;
        }
    }

    public static void Initialize(IntPtr nativeDelegates)
    {
        if (IsInitialized)
        {
            throw new AlreadyInitializedException();
        }

        _nativeDelegates = *(NativeDelegates*)nativeDelegates;

        IsInitialized = true;
    }
}
