#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponDataAsset.h"
#include "WeaponInterface.h"
#include "GunBase.generated.h"

UCLASS()
class XV_API AGunBase : public AActor, public IWeaponInterface
{
	GENERATED_BODY()

public:
	AGunBase();
	
	virtual void BeginPlay() override;

	virtual void FireBullet() override;
	virtual void Reload() override;
	virtual bool IsReloading() const override;
	bool IsFire() const;
	
	virtual int32 GetCurrentAmmo() const override;

	virtual UWeaponDataAsset* GetWeaponData() const override { return WeaponDataAsset; }

	// 애니메이션
	virtual UAnimMontage* GetEquipMontage() const override;
	virtual UAnimMontage* GetFireMontage() const override;
	virtual UAnimMontage* GetReloadMontage() const override;

protected:
	FVector GetAimDirection() const;
	FVector GetMuzzleLocation() const;
	
	void SpawnBullet();
	void PlayEffects();
	void PlaySoundAtMuzzle(USoundBase* Sound);
	void FinishReload();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	USkeletalMeshComponent* GunMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Data")
	UWeaponDataAsset* WeaponDataAsset;

	int32 CurrentAmmo;
	int32 RemainingAmmo;
	bool bIsReloading;
	bool bCanFire;

	FTimerHandle FireCooldownHandle;
	FTimerHandle ReloadTimerHandle;
};
