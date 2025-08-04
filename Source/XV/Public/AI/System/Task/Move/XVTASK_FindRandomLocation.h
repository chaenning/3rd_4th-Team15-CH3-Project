#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "XVTASK_FindRandomLocation.generated.h"

struct FBlackboardKeySelector;
/**
 * 
 */
UCLASS(Blueprintable)
class XV_API UXVTASK_FindRandomLocation : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UXVTASK_FindRandomLocation();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector LocationKey;

	UPROPERTY(EditAnywhere, Category = "Search", meta = (ClampMin = "100.0"))
	float SearchRadius = 1000.0f;
};
