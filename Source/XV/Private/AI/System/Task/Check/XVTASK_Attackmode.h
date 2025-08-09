#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "XVTASK_Attackmode.generated.h"

/**
 * 
 */
UCLASS()
class XV_API UXVTASK_Attackmode : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UXVTASK_Attackmode();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector MyLocationKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector PlayerLocationKey;
	
private:
	// MoveTo 호출 시간 제한을 위한 변수
	UPROPERTY()
	float LastMoveTime;
};
