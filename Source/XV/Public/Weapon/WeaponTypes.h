#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None     UMETA(DisplayName = "None"),
	Pistol   UMETA(DisplayName = "Pistol"),
	Rifle    UMETA(DisplayName = "Rifle"),
	ShotGun    UMETA(DisplayName = "ShotGun"),
};

class XV_API WeaponTypes
{
public:
};
