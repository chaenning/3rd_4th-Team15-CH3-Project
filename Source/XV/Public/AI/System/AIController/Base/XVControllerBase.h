#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "XVControllerBase.generated.h"

class UAISenseConfig_Hearing;
class UAISenseConfig_Sight;
class UAIPerceptionComponent;
class UBlackboardComponent;
class UXVDataAssetBase;
class UXVBlackBoardDataBase;

UCLASS()
class XV_API AXVControllerBase : public AAIController
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	uint8 TeamID = 1; // 원하는 팀 ID (예: 0=플레이어, 1=적AI)
public:
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	
#pragma region LifeCycle // 라이프 사이클 관련 함수 모음
//---------------------------------------------------------------------------------------------------------------------//
public:
	AXVControllerBase();
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
#pragma endregion
	
#pragma region AIAction // AI 행동 관련 바인딩 함수
//---------------------------------------------------------------------------------------------------------------------//
protected:
	
	UFUNCTION()
	void OnTargetInfoUpdated(AActor* Actor, FAIStimulus Stimulus); // 감지한 타겟 정보 업데이트
	
#pragma endregion 

#pragma region Functions // 기타 함수들 모음 (리플렉션 등록 X)
//---------------------------------------------------------------------------------------------------------------------//
public:
	// 블랙 보드 getter 
	FORCEINLINE UBlackboardComponent* GetBlackboardComp() const { return AIBlackBoard; }

private:
	// DataAsset 값들을 로그로 출력하는 함수 (퍼셉션 관련 필수만)
	void LogDataAssetValues() const;
#pragma endregion 
	
#pragma region Var_And_Components // 변수 및 컴포넌트 모음
//---------------------------------------------------------------------------------------------------------------------//

public:
	// 블랙보드 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | AIBlackboardAsset")
	TObjectPtr<UXVBlackBoardDataBase> AIBlackboardAsset;

	// 비해비어 트리
	UPROPERTY(EditDefaultsOnly, Category = "AI | Components")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;
	
	// 퍼셉션 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI | Components")
	TObjectPtr<UAIPerceptionComponent> AIPerception;

	// 눈
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI | Components")
	TObjectPtr<UAISenseConfig_Sight> AISightConfig;

	// 귀
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI | Components")
	TObjectPtr<UAISenseConfig_Hearing> AIHearingConfig;

	// 블랙 보드 컴포넌트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI | Components")
	TObjectPtr<UBlackboardComponent> AIBlackBoard;
#pragma endregion 
};