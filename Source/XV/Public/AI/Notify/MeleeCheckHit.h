#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "MeleeCheckHit.generated.h"

class AAIWeaponMeleeBase;
/**
 * 
 */
UCLASS()
class XV_API UMeleeCheckHit : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	
protected:
	UPROPERTY()
	AAIWeaponMeleeBase* CurrentWeapon;
	
};
