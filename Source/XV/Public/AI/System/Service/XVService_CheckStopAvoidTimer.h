#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "XVService_CheckStopAvoidTimer.generated.h"

/**
 * 
 */
UCLASS()
class XV_API UXVService_CheckStopAvoidTimer : public UBTService
{
	GENERATED_BODY()
	
public:
	UXVService_CheckStopAvoidTimer();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	
	UPROPERTY(EditAnywhere, Category = "Search")
	float StopAvoidTime = 5.0f;

	float StartTimestamp = 0.0f;

	float AccumulatedTime;
	
};
