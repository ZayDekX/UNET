#pragma once

#include <CoreMinimal.h>

#include "../ThirdParty/corehost/hostfxr.h"
#include "../ThirdParty/corehost/coreclr_delegates.h"

UNET_API DECLARE_LOG_CATEGORY_EXTERN(LogHostFXR, Error, All);

class HostFXR {

    void* Handle = nullptr;
    hostfxr_initialize_for_runtime_config_fn _initializeForRuntimeConfig = nullptr;
    hostfxr_get_runtime_delegate_fn _getRuntimeDelegate = nullptr;
    hostfxr_close_fn _closeRuntime = nullptr;

public:

    bool IsActive() const {
        return Handle && _initializeForRuntimeConfig && _getRuntimeDelegate && _closeRuntime;
    }

    HostFXR() {}

    ~HostFXR();

    hostfxr_handle InitForRuntimeConfig(FString ConfigPath) const;

    void Load(FString HostPath);
    void Unload();

    void* LoadRuntimeAndGetFunctionPointer(hostfxr_handle RuntimeHandle, FString AssemblyPath, FString EntryType, FString EntryMethod) const;

    void CloseRuntime(hostfxr_handle RuntimeHandle) const;
};
