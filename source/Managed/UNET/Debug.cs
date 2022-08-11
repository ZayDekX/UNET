using UNET.Interop;

using SystemDebug = System.Diagnostics.Debug;

namespace UNET;

public static class Debug
{
    public static unsafe void Log(ELogVerbosity level, string? message)
    {
        if (string.IsNullOrWhiteSpace(message))
        {
            return;
        }

        Log(message);

        if (!Core.IsInitialized)
        {
            return;
        }

        var memory = message.AsMemory();

        var messageHandle = memory.Pin();

        Core.NativeDelegates.Log(level, (nint)messageHandle.Pointer, message.Length);

        messageHandle.Dispose();
    }

    public static void Log(string? message) => SystemDebug.WriteLine(message);
}
