#pragma once

#include "CoreMinimal.h"
#include "AI/Weapons/Base/AIWeaponBase.h"
#include "AIWeaponMeleeBase.generated.h"

UCLASS()
class XV_API AAIWeaponMeleeBase : public AAIWeaponBase
{
	GENERATED_BODY()
	
public:
	AAIWeaponMeleeBase();
	void CheckMeleeHit();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "AI")
	TObjectPtr<UAudioComponent> NoHitSound;	
};
