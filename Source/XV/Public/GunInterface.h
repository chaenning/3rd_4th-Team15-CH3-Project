// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GunInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGunInterface : public UInterface
{
	GENERATED_BODY()
};

class XV_API IGunInterface
{
	GENERATED_BODY()

public:
	virtual void FireBullet() = 0;
	virtual void Reload() = 0;
	virtual FName GetGunType() const = 0;
	
	UFUNCTION()
	virtual void OnGunOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) = 0;
	
	UFUNCTION()
	virtual void OnGunEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) = 0;
};
