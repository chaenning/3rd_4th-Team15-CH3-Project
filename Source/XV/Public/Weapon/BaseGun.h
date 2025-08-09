// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunInterface.h"
#include "WeaponStat.h"
#include "WeaponTypes.h"
#include "GameFramework/Actor.h"
#include "BaseGun.generated.h"


UCLASS()
class XV_API ABaseGun : public AActor, public IGunInterface
{
	GENERATED_BODY()
	
public:	
	ABaseGun();

	virtual EWeaponType GetWeaponType();
	virtual void FireBullet() override;

protected:
	int32 CurrentAmmo; // 현재 탄약
	bool bIsReloading; // 장전 중 여부

	FTimerHandle ReloadTimerHandle;

	void StartReload(); // 장전 시작
	void FinishReload(); // 장전 완료
	
	virtual void BeginPlay() override;

	EWeaponType CurrentWeaponType;

	UPROPERTY(EditDefaultsOnly, Category="Weapon Data")
	UDataTable* WeaponDataTable;

	UPROPERTY(EditAnywhere, Category="Weapon Data")
	FName WeaponRowName;

	FWeaponStat WeaponStat;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* GunMesh;

	void LoadWeaponData();
	
	virtual void Reload() override;

	UFUNCTION()
	virtual void OnGunOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
	UFUNCTION()
	virtual void OnGunEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;


	virtual FName GetGunType() const override;
	
};
