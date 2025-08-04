#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "XVService_IsTooFar.generated.h"

/**
 * 
 */
UCLASS()
class XV_API UXVService_IsTooFar : public UBTService
{
	GENERATED_BODY()
		
public:
	UXVService_IsTooFar();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
	UPROPERTY(EditAnywhere, Category = "Check")
	float TooFarDistance = 1000.0f;

};
