#include "UNETClass.h"
#include "Delegates.h"

UUNETClass::UUNETClass(FManagedClassInfo* Info) :
    UClass(
        EC_StaticConstructor,
        Info->ClassName,
        Info->PropertiesSize,
        Info->MinAlignment,
        (EClassFlags)Info->ClassFlags,
        Info->CastFlags,
        (TCHAR*)Info->ClassConfigNameUTF8,
        RF_Public | RF_Standalone | RF_Transient | RF_MarkAsNative | RF_MarkAsRootSet,
        Info->BaseClass->ClassConstructor,
        Info->BaseClass->ClassVTableHelperCtorCaller,
        Info->BaseClass->ClassAddReferencedObjects
    ) {}

/**
* Called by C# generated static boilerplate code
*/
static UClass* UNET::OuterRegisterInternal(FManagedClassInfo* info)
{
    if (!info->RegistrationInfo->OuterSingleton)
    {
        UECodeGen_Private::ConstructUClass(info->RegistrationInfo->OuterSingleton, *info);
    }
    return info->RegistrationInfo->OuterSingleton;
}

/**
* Called by C# generated static boilerplate code
*/
static UClass* UNET::InnerRegisterInternal(FManagedClassInfo* info)
{
    if (!info->RegistrationInfo->InnerSingleton)
    {
        auto ReturnClass = ::new (GUObjectAllocator.AllocateUObject(sizeof(UUNETClass), alignof(UUNETClass), true)) UUNETClass(info);

        check(ReturnClass);

        InitializePrivateStaticClass(
            info->BaseClass,
            ReturnClass,
            info->BaseClass->ClassWithin,
            info->PackageName,
            info->ClassName
        );

        info->RegistrationInfo->InnerSingleton = ReturnClass;
    }
    return info->RegistrationInfo->InnerSingleton;
}

static void UNET::RegisterNewClass(FManagedClassInfo* Info) {

    Info->Initialize();

    RegisterCompiledInInfo(
        Info->OuterRegister,
        Info->InnerRegister,
        Info->PackageName,
        Info->ClassName,
        *Info->RegistrationInfo,
        Info->RegistrationInfo->ReloadVersionInfo);

    Info->IsRegistered = true;
}
