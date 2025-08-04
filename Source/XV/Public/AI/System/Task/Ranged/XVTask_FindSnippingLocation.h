#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "XVTask_FindSnippingLocation.generated.h"

UCLASS()
class XV_API UXVTask_FindSnippingLocation : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UXVTask_FindSnippingLocation();
	
protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector SnippingLocationKey;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category = "Search")
	float SearchRadius = 1000.0f;
	UPROPERTY(EditAnywhere, Category = "Search")
	float MinRange = 200.0f;
	UPROPERTY(EditAnywhere, Category = "Search")
	float MaxRange = 500.0f;
};
