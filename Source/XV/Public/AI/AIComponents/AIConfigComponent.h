#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIConfigComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XV_API UAIConfigComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UAIConfigComponent();

public:
//=== AI 세팅 함수 ======================================================================================================//	
	UFUNCTION(BlueprintCallable)
	void ConfigSetting();

public:
// === AI 공격 가능 범위 ================================================================================================//	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Setting")
	float AttackRange;
	
//=== 귀 설정 (AI) =====================================================================================================//	
public:
	// 청각 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Setting")
	float AIHearingRange;
	
	// 청각 기억력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Setting")
	float AIHearingSetMaxAge;
	
//=== 눈 설정 ===========================================================================================================//
public:
	// 시야 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Setting")
	float AISightRadius;

	// 시야를 잃는 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Setting")
	float AILoseSightRadius;

	// 시야각
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Setting")
	float AIPeripheralVisionAngleDegrees;

	// 시야 기억력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Setting")
	float AISightSetMaxAge;
	
//===적 감지 관련 ===========================================================================================================//
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Setting")
	bool AIbDetectEnemies;

	// 중립 감지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Setting")
	bool AIbDetectNeutrals;

	// 아군 감지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Setting")
	bool AIbDetectFriendlies;
};
