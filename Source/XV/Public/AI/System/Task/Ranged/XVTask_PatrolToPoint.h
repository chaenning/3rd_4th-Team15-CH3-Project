#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "XVTask_PatrolToPoint.generated.h"

UCLASS()
class XV_API UXVTask_PatrolToPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UXVTask_PatrolToPoint();

protected:
	EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);
};
