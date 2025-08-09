#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Weapon/WeaponTypes.h"
#include "WeaponAnimRow.generated.h"

USTRUCT(BlueprintType)
struct FWeaponAnimRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EWeaponType WeaponType;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* EquipAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* FireAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UAnimMontage* ReloadAnimMontage;
};
