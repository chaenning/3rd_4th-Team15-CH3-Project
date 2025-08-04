#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "XVTASK_CheckSnippingBeforeMove.generated.h"

/**
 * 
 */
UCLASS()
class XV_API UXVTASK_CheckSnippingBeforeMove : public UBTTaskNode
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Search")
	float CheckDistance = 1500.0f;
};
