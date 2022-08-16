#pragma once

#include <CoreMinimal.h>

#include "UNETClass.h"

UNET_API DECLARE_LOG_CATEGORY_EXTERN(LogUNETManaged, Log, All);

namespace UNET {

    static void LogManaged(ELogVerbosity::Type Level, TCHAR* Message);
    static UClass* OuterRegisterInternal(FManagedClassInfo* Info);
    static UClass* InnerRegisterInternal(FManagedClassInfo* Info);
    static void RegisterNewClass(FManagedClassInfo* Info);

    static const struct NativeDelegates {
        void(__cdecl* _log)(ELogVerbosity::Type, TCHAR*) = &UNET::LogManaged;
        UClass* (__cdecl* _outerRegisterInternal)(FManagedClassInfo*) = &OuterRegisterInternal;
        UClass* (__cdecl* _innerRegisterInternal)(FManagedClassInfo*) = &InnerRegisterInternal;
        void(__cdecl* _registerManagedClass)(FManagedClassInfo*) = &RegisterNewClass;
    } NativeDelegates;

    // Loaded on C# side
    static struct PluginLoaderDelegates {
        void(__cdecl* Load)();
        void(__cdecl* Unload)();
        void(__cdecl* Reload)();
    } PluginLoaderDelegates;
}
