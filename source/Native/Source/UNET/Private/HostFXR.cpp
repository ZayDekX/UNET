#include "HostFXR.h"

DEFINE_LOG_CATEGORY(LogHostFXR);

static void WriteError(const TCHAR* text) {
    UE_LOG(LogHostFXR, Error, TEXT("%s"), text);
}

void HostFXR::Load(FString HostPath)
{
    Handle = FPlatformProcess::GetDllHandle(*HostPath);

    _initializeForRuntimeConfig = (hostfxr_initialize_for_runtime_config_fn)FPlatformProcess::GetDllExport(Handle, TEXT("hostfxr_initialize_for_runtime_config"));
    _getRuntimeDelegate = (hostfxr_get_runtime_delegate_fn)FPlatformProcess::GetDllExport(Handle, TEXT("hostfxr_get_runtime_delegate"));
    _closeRuntime = (hostfxr_close_fn)FPlatformProcess::GetDllExport(Handle, TEXT("hostfxr_close"));
    
    auto SetErrorWriter = (hostfxr_set_error_writer_fn)FPlatformProcess::GetDllExport(Handle, TEXT("hostfxr_set_error_writer"));
    SetErrorWriter(&WriteError);
}

void HostFXR::Unload() {
    if (IsActive()) {
        FPlatformProcess::FreeDllHandle(Handle);
    }

    _initializeForRuntimeConfig = nullptr;
    _getRuntimeDelegate = nullptr;
    _closeRuntime = nullptr;
    Handle = nullptr;
}

HostFXR::~HostFXR() {
    _initializeForRuntimeConfig = nullptr;
    _getRuntimeDelegate = nullptr;
    _closeRuntime = nullptr;

    FPlatformProcess::FreeDllHandle(Handle);
    Handle = nullptr;
}

hostfxr_handle HostFXR::InitForRuntimeConfig(FString ConfigPath) const {
    hostfxr_handle contextHandle;
    _initializeForRuntimeConfig(*ConfigPath, nullptr, &contextHandle);
    return contextHandle;
}

void* HostFXR::LoadRuntimeAndGetFunctionPointer(hostfxr_handle RuntimeHandle, FString AssemblyPath, FString EntryType, FString EntryMethod) const {
    load_assembly_and_get_function_pointer_fn LoadAssembly;
    _getRuntimeDelegate(RuntimeHandle, hdt_load_assembly_and_get_function_pointer, (void**)&LoadAssembly);

    void* EntryPoint;
    LoadAssembly(*AssemblyPath, *EntryType, *EntryMethod, UNMANAGEDCALLERSONLY_METHOD, nullptr, &EntryPoint);

    return EntryPoint;
}

void HostFXR::CloseRuntime(hostfxr_handle RuntimeHandle) const {
    _closeRuntime(RuntimeHandle);
}