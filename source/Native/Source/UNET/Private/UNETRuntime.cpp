#include "UNETRuntime.h"

void UNET::Runtime::Load(const HostFXR& Host, const UUNETSettings* Settings) {
    auto ConfigPath = Settings->GetUNETLoaderConfigPath(),
        AssemblyPath = Settings->GetUNETLoaderLibraryPath(),
        ManagedPluginsPath = Settings->GetManagedPluginsPath();

    Handle = Host.InitForRuntimeConfig(ConfigPath);
    auto Initialize = (Initializer)Host.LoadRuntimeAndGetFunctionPointer(Handle, AssemblyPath, Settings->EntryType, Settings->EntryMethod);
    Initialize(*ManagedPluginsPath, ManagedPluginsPath.Len(), &UNET::NativeDelegates, &UNET::PluginLoaderDelegates);
}

void UNET::Runtime::Unload(const HostFXR& Host) {
    Host.CloseRuntime(Handle);
    Handle = nullptr;
}