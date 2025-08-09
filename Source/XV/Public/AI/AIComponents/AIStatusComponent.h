#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIStatusComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XV_API UAIStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAIStatusComponent();

public:
	virtual void TakeDamage(float Damage);
	
private:
	UPROPERTY(EditAnywhere, Category = "AI Status")
	float Health;
	
public:
	UPROPERTY(EditAnywhere, Category = "AI Status")
	float AttackDamage;

};
