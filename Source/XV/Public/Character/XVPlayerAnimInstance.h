// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "XVPlayerAnimInstance.generated.h"


UCLASS()
class XV_API UXVPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()	
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "PlayerAnim")
	class UAnimMontage* AttackAnimMontage;
	UPROPERTY(EditDefaultsOnly, Category = "PlayerAnim")
	class UAnimMontage* ChangeAnimMontage;
	
	void PlayAttackAnim();
	void PlayGunChangeAnim();
};
