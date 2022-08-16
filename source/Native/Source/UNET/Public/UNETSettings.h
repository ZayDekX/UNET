#pragma once

#include <CoreMinimal.h>

#include <Engine/EngineTypes.h>
#include <Engine/DeveloperSettings.h>
#include <GenericPlatform/GenericPlatformFile.h>

#include "Version.h"
#include "LogUNET.h"
#include "UNETSettings.generated.h"

#define MINIMAL_DOTNET_VERSION 6

#if PLATFORM_WINDOWS 
#define DEFAULT_DOTNET_INSTALLDIR TEXT("C:/Program Files/dotnet/")
#define HOSTFXR_LIB "hostfxr.dll"
#elif PLATFORM_LINUX
#define DEFAULT_DOTNET_INSTALLDIR TEXT("/home/user/share/dotnet/")
#define HOSTFXR_LIB "libhostfxr.so"
#elif PLATFORM_MAC
#define DEFAULT_DOTNET_INSTALLDIR TEXT("/usr/local/share/dotnet/")
#define HOSTFXR_LIB "libhostfxr.dylib"
#endif

using namespace UC;
using namespace UP;
using namespace UM;
using namespace UF;

/**
* Configure UNET
*/
UCLASS(config = Engine, defaultconfig)
class UUNETSettings : public UDeveloperSettings {

    GENERATED_BODY()

private:

    FString GetDotnetInstallDir() {
        return 
#if WITH_EDITOR
        GetEditorDotnetInstallDir();
#else
        GetEmbeddedDotnetInstallDir();
#endif
    }

    FString GetEmbeddedDotnetInstallDir() {
        return FPaths::ConvertRelativePathToFull(FPaths::Combine(FPaths::ProjectDir(), "Binaries/Runtime"));
    }

#if WITH_EDITOR
    FString GetEditorDotnetInstallDir() {

#ifdef DEFAULT_DOTNET_INSTALLDIR
        
        if (FPaths::DirectoryExists(DEFAULT_DOTNET_INSTALLDIR))
        {
            return DEFAULT_DOTNET_INSTALLDIR;
        }

#endif

        return GetEmbeddedDotnetInstallDir();
    }
#endif

    void UpdateDotnetInstallations() {

        AvailableDotNetInstallations.Empty();

        auto HostDir = GetHostfxrPath();

        auto AddDirectory = [&](const TCHAR* Path, bool bIsDir) -> bool {
            if (!bIsDir) {
                return true;
            }

            auto PathStr = FString(Path);

            auto DirName = PathStr.Right(PathStr.Len() - HostDir.Len() - 1);

            FVersion FoundVersion;

            if (!FVersion::TryParse(DirName, FoundVersion) ||
                (FoundVersion.Major < MINIMAL_DOTNET_VERSION) ||
                (FoundVersion.bIsPreview && !bAllowDotNetPreview)) {
                return true;
            }

            AvailableDotNetInstallations.Add(DirName);

            return true;
        };

        FPlatformFileManager::Get().GetPlatformFile().IterateDirectory(*HostDir, AddDirectory);
    }

    TArray<FString> AvailableDotNetInstallations;

public:

#if WITH_EDITOR
    //~ UObject interface
    virtual bool CanEditChange(const FProperty* InProperty) const override;
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

    //~ UDeveloperSettings interface
    virtual FText GetSectionText() const override;

#endif

    virtual FName GetCategoryName() const override
    {
        return TEXT("Plugins");
    }

    UUNETSettings(const FObjectInitializer& ObjectInitializer);
    
    /**
    * Path to .NET installations    
    */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = ".NET", meta = (DisplayName = "Path to .NET installations"))
    FDirectoryPath DotNetLocation;
    
    /**
    * Version of .NET which will be used for current project
    */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = ".NET", meta = (DisplayName = ".NET version", GetOptions = "GetDotnetInstallations"))
    FString DotNetVersion;

    /**
    * Allow preview versions of .NET
    */
    UPROPERTY(config, EditAnywhere, BlueprintReadOnly, Category = ".NET", AdvancedDisplay, meta = (DisplayName = "Allow preview"))
    bool bAllowDotNetPreview;

    UFUNCTION()
    TArray<FString> GetDotnetInstallations() const {
        return AvailableDotNetInstallations;
    }

    FString GetProjectPath() const {
        return FPaths::ConvertRelativePathToFull(FPaths::ProjectDir());
    }

    FString GetHostfxrPath() const {
        return FPaths::Combine(DotNetLocation.Path, TEXT("host/fxr"));
    }

    FString GetHostfxrLibPath() const {
        return FPaths::Combine(GetHostfxrPath(), DotNetVersion, HOSTFXR_LIB);
    }

    FString GetManagedPluginsPath() const {
        return FPaths::Combine(GetProjectPath(), "Binaries/Managed");
    }

    FString GetUNETLoaderLibraryPath() const {
        return FPaths::Combine(GetManagedPluginsPath(), "UNET.Plugins.dll");
    }

    FString GetUNETLoaderConfigPath() const {
        return FPaths::Combine(GetManagedPluginsPath(), "UNET.Plugins.runtimeconfig.json");
    }
    
    const FString EntryType = FString("UNET.Plugins.Loader, UNET.Plugins");

    const FString EntryMethod = FString("Init");

    bool Validate() const;
};

#undef MINIMAL_DOTNET_VERSION
#undef DEFAULT_DOTNET_INSTALLDIR
#undef HOSTFXR_LIB