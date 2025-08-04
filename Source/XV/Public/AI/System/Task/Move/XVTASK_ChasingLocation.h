#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "XVTASK_ChasingLocation.generated.h"

struct FBlackboardKeySelector;
/**
 * 
 */
UCLASS(Blueprintable)
class XV_API UXVTASK_ChasingLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UXVTASK_ChasingLocation();
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
