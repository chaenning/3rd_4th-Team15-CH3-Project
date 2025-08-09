// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WeaponStat.generated.h"

USTRUCT(BlueprintType)
struct FWeaponStat : public FTableRowBase
{
	GENERATED_BODY()

	// 무기 이름 (표시용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName WeaponName;

	// 무기 종류 (ex. Rifle, Pistol)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName WeaponType;

	// 최대 탄약 수
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxAmmo;

	// 데미지 수치
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	// 발사 속도 (초당 발사 횟수)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireRate;

	// 총기 메쉬 (옵션)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* WeaponMesh;

	// 총기 블루프린트 클래스 (스폰용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class ABaseGun> WeaponClass;
};
