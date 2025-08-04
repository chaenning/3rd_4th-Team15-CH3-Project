#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardData.h"
#include "XVBlackBoardDataBase.generated.h"

/**
 * 
 */
UCLASS()
class XV_API UXVBlackBoardDataBase : public UBlackboardData
{
	GENERATED_BODY()
public:
	UXVBlackBoardDataBase();
	
// ===	블랙보드에 담을 데이터 설정 (BlackBoard) ==========================================================================//
public:
	// 타겟 탐지 여부 (기본 false)
	bool AICanSeeTarget = false;

	// 조사 중 상태 초기화 (기본 false)
	bool AIIsInvestigating = false;

	// 공격 상태 여부 (기본 false)
	bool AIIsAttacking = false;
};
