#pragma once

#include <CoreMinimal.h>
#include <UObject/UObjectAllocator.h>

#include "ManagedClassInfo.h"

class UNET_API UUNETClass : public UClass {
public:
    UUNETClass(FManagedClassInfo* Info);
};