#pragma once

#include <UObject/UObjectGlobals.h>

//   Note: Created only on C# side and passed to C++ by pointer, so here it doesn't need a constructor.
/**
 *   Information about managed class that will be constructed.
 */
struct FManagedClassInfo : UECodeGen_Private::FClassParams {

private:
    void SetupProperties();

public:
    EClassCastFlags CastFlags;

    const TCHAR* PackageName;
    const TCHAR* ClassName;
    const TCHAR* ParentName;

    FClassRegistrationInfo* RegistrationInfo;

    // gets class info without registration
    UClass* (__cdecl* InnerRegister)();
    // gets class info with registration
    UClass* (__cdecl* OuterRegister)();

    // By default it is nullptr, because C# side doesn't know anything about UClass
    UClass* BaseClass;

    int32 PropertiesSize;
    int32 MinAlignment;

    //Flag used to determine, whether this info is already registered in Unreal Engine
    uint8 IsRegistered;

    void Initialize();
};