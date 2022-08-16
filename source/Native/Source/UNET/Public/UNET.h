#pragma once

#include <CoreMinimal.h>
#include <Modules/ModuleManager.h>

#include "LogUNET.h"
#include "UNETSettings.h"
#include "HostFXR.h"
#include "UNETRuntime.h"

class FUNETModule : public IModuleInterface
{
    bool LoadHost();
    void LoadRuntime();
    void UnloadRuntime();

    void LoadPlugins();
    void UnloadPlugins();
    void ReloadPlugins();

    HostFXR Host;
    UNET::Runtime Runtime;

public:
    
    FUNETModule();

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    FAutoConsoleCommand LoadRuntimeCommand;
    FAutoConsoleCommand UnloadRuntimeCommand;

    FAutoConsoleCommand LoadManagedPluginsCommand;
    FAutoConsoleCommand UnloadManagedPluginsCommand;
    FAutoConsoleCommand ReloadManagedPluginsCommand;
};