#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "XVEnemyBase.generated.h"

class AAIWeaponBase;
class UAIStatusComponent;
class UAIConfigComponent;
class UXVDataAssetBase;

UCLASS()
class XV_API AXVEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	AXVEnemyBase();
protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

// === 컴포넌트 ========================================================================================================//
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UAIConfigComponent> AIConfigComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	TObjectPtr<UAIStatusComponent> AIStatusComponent;
	
	UPROPERTY(EditAnywhere, Blueprintable, Category = "AI")
	TSubclassOf<AAIWeaponBase> AIWeaponBaseClass;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<AAIWeaponBase> AIWeaponBase;
	
// === 무기 관련 세팅 ===================================================================================================//
protected:
	void SetWeapon();

	
// === AI 관련 속도, 회전 세팅 ===========================================================================================//	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float RotateSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float Deceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float BrakingFriction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool ControllerDesiredRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	bool OrientRotationToMovement;

// === AI 패트롤 포인트 태스크 관련 세팅 ==================================================================================//	
public:
	// 순찰 포인트
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI")
	TArray<AActor*> PatrolPoints;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI")
	int32 CurrentPatrolIndex = 0;
	
// === AI 공격 모드 관련 세팅 ==================================================================================//	
public:
	void SetAttackMode();
protected:
	// 공격 시 속도
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI")
	float AttackModeSpeed;
};
