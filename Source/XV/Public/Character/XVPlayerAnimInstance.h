// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "XVPlayerAnimInstance.generated.h"


class AGunBase;
class UAnimMontage;

UCLASS()
class XV_API UXVPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()	
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "PlayerAnim")
	UAnimMontage* AttackAnimMontage;
	UPROPERTY(EditDefaultsOnly, Category = "PlayerAnim")
	UAnimMontage* EquipAnimMontage;
	UPROPERTY(EditDefaultsOnly, Category = "PlayerAnim")
	UAnimMontage* UnequipAnimMontage;
	UPROPERTY(EditDefaultsOnly, Category = "PlayerAnim")
	UAnimMontage* ReloadAnimMontage;
	UPROPERTY(EditDefaultsOnly, Category = "PlayerAnim")
	UAnimMontage* DieAnimMontage;
	
	void PlayAttackAnim(AGunBase* Weapon);
	void PlayWeaponEquipAnim(AGunBase* Weapon);
	void PlayWeaponUnequipAnim();
	void PlayReloadAnim(AGunBase* Weapon);
	void PlayDieAnim();
};
