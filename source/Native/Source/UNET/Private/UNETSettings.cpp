#include "UNETSettings.h"

#define LOCTEXT_NAMESPACE "UNET"

#if WITH_EDITOR
bool UUNETSettings::CanEditChange(const FProperty* InProperty) const {
    bool bCanEditChange = Super::CanEditChange(InProperty);

    auto PropertyName = InProperty->GetFName();

    if (PropertyName == GET_MEMBER_NAME_CHECKED(UUNETSettings, DotNetVersion)) {
        bCanEditChange &= FPaths::DirectoryExists(GetHostfxrPath()) & !GetDotnetInstallations().IsEmpty();
    }

    return bCanEditChange;
}

void UUNETSettings::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (!PropertyChangedEvent.MemberProperty) {
        return;
    }

    auto PropertyName = PropertyChangedEvent.MemberProperty->GetFName();

    if (PropertyName == GET_MEMBER_NAME_CHECKED(UUNETSettings, DotNetLocation) ||
        PropertyName == GET_MEMBER_NAME_CHECKED(UUNETSettings, bAllowDotNetPreview)) {
        UpdateDotnetInstallations();

        FVersion CurrentVersion;

        if (DotNetVersion.IsEmpty() ||
            !FPaths::FileExists(GetHostfxrLibPath()) ||
            (!bAllowDotNetPreview && FVersion::TryParse(DotNetVersion, CurrentVersion) && CurrentVersion.bIsPreview)) {

            DotNetVersion = GetDotnetInstallations().IsEmpty() ? "" : GetDotnetInstallations().Last();

            UpdateSinglePropertyInConfigFile(
                GetClass()->FindPropertyByName(GET_MEMBER_NAME_CHECKED(UUNETSettings, DotNetVersion)),
                GetDefaultConfigFilename());
        }
    }
}

FText UUNETSettings::GetSectionText() const
{
    return LOCTEXT("SettingsDisplayName", "UNET");
}
#endif

UUNETSettings::UUNETSettings(const FObjectInitializer& ObjectInitializer) :
    UDeveloperSettings(ObjectInitializer)
{
    CategoryName = TEXT("Plugins");
    SectionName = TEXT("UNET");

    // Initialize default values
    bAllowDotNetPreview = false;
    DotNetLocation.Path = GetDotnetInstallDir();

    LoadConfig();
    UpdateDotnetInstallations();
}

bool UUNETSettings::Validate() const {
    auto managedPluginsPath = GetManagedPluginsPath();
    if (!FPaths::DirectoryExists(managedPluginsPath)) {

        UE_LOG(LogUNET, Warning, TEXT("Failed to locate directory for managed plugins"));
        auto& fileManager = FPlatformFileManager::Get().GetPlatformFile();

        if (fileManager.CreateDirectory(*managedPluginsPath)) {
            UE_LOG(LogUNET, Display, TEXT("Managed plugins directory created: %s"), *managedPluginsPath);
        }
        else
        {
            UE_LOG(LogUNET, Display, TEXT("Failed to create managed plugins directory: %s"), *managedPluginsPath);
            return false;
        }
    }

    if (!FPaths::FileExists(GetHostfxrLibPath())) {
        UE_LOG(LogUNET, Error, TEXT("Failed to locate .NET host"));
        return false;
    }

    if (!FPaths::FileExists(GetUNETLoaderLibraryPath())) {
        UE_LOG(LogUNET, Error, TEXT("Failed to locate UNET Plugin Loader"));
        return false;
    }

    if (!FPaths::FileExists(GetUNETLoaderConfigPath())) {
        UE_LOG(LogUNET, Error, TEXT("Failed to locate UNET Plugin Loader Config"));
        return false;
    }

    return true;
}

#undef LOCTEXT_NAMESPACE