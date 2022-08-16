#include "UNET.h"

#define LOCTEXT_NAMESPACE "FUNETModule"

DEFINE_LOG_CATEGORY(LogUNET);

FUNETModule::FUNETModule() :
    LoadRuntimeCommand(
        TEXT("UNET.LoadRuntime"),
        TEXT("Load .NET runtime"),
        FConsoleCommandDelegate::CreateRaw(this, &FUNETModule::LoadRuntime)),
    UnloadRuntimeCommand(
        TEXT("UNET.UnloadRuntime"),
        TEXT("Unload .NET runtime"),
        FConsoleCommandDelegate::CreateRaw(this, &FUNETModule::UnloadRuntime)),
    LoadManagedPluginsCommand(
        TEXT("UNET.LoadManagedPlugins"),
        TEXT("Load UNET Plugins"),
        FConsoleCommandDelegate::CreateRaw(this, &FUNETModule::LoadPlugins)),
    UnloadManagedPluginsCommand(
        TEXT("UNET.UnloadManagedPlugins"),
        TEXT("Unload UNET Plugins"),
        FConsoleCommandDelegate::CreateRaw(this, &FUNETModule::UnloadPlugins)),
    ReloadManagedPluginsCommand(
        TEXT("UNET.ReloadManagedPlugins"),
        TEXT("Reload UNET Plugins"),
        FConsoleCommandDelegate::CreateRaw(this, &FUNETModule::ReloadPlugins))
{ }

void FUNETModule::StartupModule() {
    LoadRuntime();
}

void FUNETModule::ShutdownModule() {
    UnloadRuntime();
}

void FUNETModule::LoadPlugins() {
    if (!Runtime.IsActive()) {
        UE_LOG(LogUNET, Error, TEXT("UNET Runtime is not loaded"));
        return;
    }

    UNET::PluginLoaderDelegates.Load();
}

void FUNETModule::ReloadPlugins() {
    if (!Runtime.IsActive()) {
        UE_LOG(LogUNET, Error, TEXT("UNET Runtime is not loaded"));
        return;
    }

    UNET::PluginLoaderDelegates.Reload();
}

void FUNETModule::UnloadPlugins() {
    if (!Runtime.IsActive()) {
        UE_LOG(LogUNET, Error, TEXT("UNET Runtime is not loaded"));
        return;
    }

    UNET::PluginLoaderDelegates.Unload();
}

FORCENOINLINE bool FUNETModule::LoadHost() {
    if (Host.IsActive()) {
        UE_LOG(LogUNET, Warning, TEXT("HostFXR is already loaded"));
        return true;
    }

    auto Settings = GetDefault<UUNETSettings>();
    if (!Settings->Validate()) {
        UE_LOG(LogUNET, Error, TEXT("Failed to load HostFXR"));
        return false;
    }

    Host.Load(Settings->GetHostfxrLibPath());
    UE_LOG(LogUNET, Display, TEXT("HostFXR is loaded for .NET %s"), *Settings->DotNetVersion);
    return true;
}

void FUNETModule::LoadRuntime() {
    if (Runtime.IsActive()) {
        UE_LOG(LogUNET, Warning, TEXT("UNET Runtime is already loaded"));
        return;
    }

    auto Settings = GetDefault<UUNETSettings>();

    if (!Settings->Validate() || !Host.IsActive() && !LoadHost()) {
        return;
    }

    Runtime.Load(Host, Settings);

    if (!Runtime.IsActive()) {
        UE_LOG(LogUNET, Error, TEXT("Failed to load UNET runtime"));
        return;
    }

    UE_LOG(LogUNET, Display, TEXT("UNET Runtime is loaded"));
    LoadPlugins();
}

void FUNETModule::UnloadRuntime() {
    if (!Runtime.IsActive()) {
        UE_LOG(LogUNET, Warning, TEXT("UNET Runtime was not loaded"));
        return;
    }

    Runtime.Unload(Host);
    UE_LOG(LogUNET, Display, TEXT("UNET Runtime is unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FUNETModule, UNET)