#include "ManagedClassInfo.h"

void FManagedClassInfo::Initialize() {
    BaseClass = (UClass*)StaticFindObject(UObject::StaticClass(), ANY_PACKAGE, ParentName, false);

    check(BaseClass);

    SetupProperties();
}

void FManagedClassInfo::SetupProperties() {
    PropertiesSize = BaseClass->PropertiesSize; // position in structure
    MinAlignment = BaseClass->MinAlignment;

    for (int i = 0; i < NumProperties; i++) {
        auto propertyInfo = (UECodeGen_Private::FPropertyParamsBaseWithOffset*)(PropertyArray[i]);

        // we write size of property to offset in C# (not good, but works)
        auto propertySize = propertyInfo->Offset;

        // from C# we can write only basic values or pointers, so alignment is the same as size
        propertyInfo->Offset = Align(PropertiesSize, propertySize);
        PropertiesSize += propertySize;
        MinAlignment = FMath::Max(MinAlignment, propertySize);
    }
}