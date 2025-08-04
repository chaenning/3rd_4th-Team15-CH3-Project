#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "XVTASK_IsPlayerClosed_ForAviod.generated.h"

UCLASS()
class XV_API UXVTASK_IsPlayerClosed_ForAviod : public UBTTaskNode
{
	GENERATED_BODY()
public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Search")
	float MoveDistance = 500.0f;
};