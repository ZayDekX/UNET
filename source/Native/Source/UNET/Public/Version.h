#pragma once

#include <CoreMinimal.h>

struct FVersion {

    FVersion(int8 Major = 0, int8 Minor = 0, int8 Patch = 0, int8 Hotfix = 0, bool bIsPreview = false) :
        Major(Major),
        Minor(Minor),
        Patch(Patch),
        Hotfix(Hotfix),
        bIsPreview(bIsPreview) {}

    union {
        struct {
            uint8 Major;
            uint8 Minor;
            uint8 Patch;
            uint8 Hotfix;
        };

        uint32 Inlined;
    };

    bool bIsPreview;

    static bool TryParse(FString Input, FVersion& Result) {
        FString MajorStr, MinorStr, PatchStr, HotfixStr, Preview;

        auto Success = Input.Split(TEXT("."), &MajorStr, &MinorStr) && MinorStr.Split(TEXT("."), &MinorStr, &PatchStr);
        auto bIsPreview = PatchStr.Split(TEXT("-preview."), &PatchStr, &Preview);

        auto bHasHotfix = PatchStr.Split(TEXT("."), &PatchStr, &HotfixStr);

        if (!Success)
        {
            return false;
        }

        Result = FVersion(
            atoi(TCHAR_TO_ANSI(*MajorStr)),
            atoi(TCHAR_TO_ANSI(*MinorStr)),
            atoi(TCHAR_TO_ANSI(*PatchStr)),
            bHasHotfix ? atoi(TCHAR_TO_ANSI(*HotfixStr)) : 0,
            bIsPreview);

        return Success;
    }

    friend bool operator <(const FVersion& lhs, const FVersion& rhs) {
        return lhs.Inlined < rhs.Inlined;
    }

    friend bool operator >(const FVersion& lhs, const FVersion& rhs) {
        return lhs.Inlined > rhs.Inlined;
    }

    friend bool operator <=(const FVersion& lhs, const FVersion& rhs) {
        return lhs.Inlined < rhs.Inlined || lhs == rhs;
    }

    friend bool operator >=(const FVersion& lhs, const FVersion& rhs) {
        return lhs.Inlined > rhs.Inlined || lhs == rhs;
    }

    friend bool operator ==(const FVersion& lhs, const FVersion& rhs) {
        return lhs.Inlined == rhs.Inlined && !(lhs.bIsPreview ^ rhs.bIsPreview);
    }

    friend bool operator !=(const FVersion& lhs, const FVersion& rhs) {
        return lhs.Inlined == rhs.Inlined && !(lhs.bIsPreview ^ rhs.bIsPreview);
    }

    FString ToString() {
        return Hotfix ?
            FString::Format(TEXT("{0}.{1}.{2}.{3}"), { Major, Minor, Patch, Hotfix }) :
            FString::Format(TEXT("{0}.{1}.{2}"), { Major, Minor, Patch });
    }
};
