#pragma once

#include <CoreMinimal.h>

#include "UNETSettings.h"
#include "Delegates.h"
#include "HostFXR.h"

namespace UNET {

    class Runtime {

        typedef void(__cdecl* Initializer)(
            const TCHAR* managedPluginsPath,
            int pathLength,
            const struct NativeDelegates* nativeDelegates,
            const struct PluginLoaderDelegates* pluginLoaderDelegates
        );

        hostfxr_handle Handle = nullptr;

    public:

        Runtime() {}

        bool IsActive() {
            return !!Handle;
        }

        void Load(const HostFXR& Host, const UUNETSettings* Settings);
        void Unload(const HostFXR& Host);
    };
}