#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

class XV_API IWeaponInterface
{
	GENERATED_BODY()

public:
	virtual void FireBullet() = 0;
	virtual void Reload() = 0;
	virtual bool IsReloading() const = 0;
	virtual int32 GetCurrentAmmo() const = 0;

	virtual class UWeaponDataAsset* GetWeaponData() const = 0;

	// 애니메이션
	virtual class UAnimMontage* GetEquipMontage() const = 0;
	virtual class UAnimMontage* GetFireMontage() const = 0;
	virtual class UAnimMontage* GetReloadMontage() const = 0;
};
