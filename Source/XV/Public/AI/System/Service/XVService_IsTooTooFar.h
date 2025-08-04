#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "XVService_IsTooTooFar.generated.h"

/**
 * 
 */
UCLASS()
class XV_API UXVService_IsTooTooFar : public UBTService
{
	GENERATED_BODY()

public:
	UXVService_IsTooTooFar();
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, Category = "Check")
	float TooTooFarDistance = 2000.0f;
};
