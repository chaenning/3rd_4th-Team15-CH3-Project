#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WeaponDataAsset.generated.h"

UCLASS(BlueprintType)
class XV_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// 무기 이름 / 타입
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "General")
	FName WeaponType;

	// 무기 메쉬
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	class USkeletalMesh* WeaponMesh;

	// 머즐 플래시
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visuals")
	class UNiagaraSystem* MuzzleFlash;

	// 탄창 최대 수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo")
	int32 MaxAmmo;

	// 재장전 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo")
	float ReloadTime;

	// 발사 속도 (초당 발사 간격)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firing")
	float WeaponRecoil;

	// 발사 속도 (초당 발사 간격)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firing")
	float FireRate;
	
	// 발사 소켓 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firing")
	FName MuzzleSocketName;

	// 발사할 탄환 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Bullet")
	TSubclassOf<class AProjectileBullet> BulletClass;

	// 발사 사운드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	class USoundBase* FireSound;

	// 재장전 사운드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	class USoundBase* ReloadSound;

	// 총알 없을 때 사운드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	class USoundBase* EmptySound;

	// 플레이어 장착 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "PlayerAnim")
	class UAnimMontage* PlayerEquipAnimMontage;

	// 플레이어 발사 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "PlayerAnim")
	class UAnimMontage* PlayerFireAnimMontage;

	// 플레이어 재장전 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "PlayerAnim")
	class UAnimMontage* PlayerReloadAnimMontage;

	// 무기 발사 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "WeaponAnim")
	class UAnimMontage* FireAnimMontage;

	// 무기 재장전 몽타주
	UPROPERTY(EditDefaultsOnly, Category = "WeaponAnim")
	class UAnimMontage* ReloadAnimMontage;
};