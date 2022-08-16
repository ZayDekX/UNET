using UNET.Interop;

namespace UNET;

internal unsafe struct NativeDelegates
{
#pragma warning disable CS0649
    private readonly delegate* unmanaged[Cdecl]<ELogVerbosity, nint, int, void> _log;
    private readonly delegate* unmanaged[Cdecl]<nint, nint> _outerRegisterInternal;
    private readonly delegate* unmanaged[Cdecl]<nint, nint> _innerRegisterInternal;
    private readonly delegate* unmanaged[Cdecl]<nint, void> _registerManagedClass;
#pragma warning restore CS0649

    public void Log(ELogVerbosity level, nint message, int length)
        => _log(level, message, length);

    public IntPtr OuterRegisterInternal(nint infoPtr)
        => _outerRegisterInternal(infoPtr);

    public IntPtr InnerRegisterInternal(nint infoPtr)
        => _innerRegisterInternal(infoPtr);

    public void RegisterManagedClass(nint infoPtr)
        => _registerManagedClass(infoPtr);
}
