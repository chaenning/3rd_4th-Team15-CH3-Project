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
	virtual float Sub_Health(float Damage){return Health = Health - Damage;}
	virtual float CurrentHealth(){return Health;}
	
	UPROPERTY(EditAnywhere, Category = "AI Status")
	float Health;

public:
	UPROPERTY(EditAnywhere, Category = "AI Status")
	float AttackDamage;

private:
	float DamagedAmount;
public:
	/* 이거 UI에 호출하는 거 아님*/
	float D_Gettter(float Damage){return DamagedAmount = Damage;}

public:
	/*UI 용 데미지 받은 양 호출 함수*/
	float DamageAmount(){return DamagedAmount;}
};
