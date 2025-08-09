#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "RangedCheckHit.generated.h"

class UNiagaraSystem;
class USoundBase;

UCLASS()
class XV_API URangedCheckHit : public UAnimNotifyState

{
	GENERATED_BODY()

public:
	// 애니메이션 노티파이 시 실행
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	void PlayHitEffects(UWorld* World, const FVector& SpawnLocation);

	// 트레이스 박스 반 사이즈(직육면체 절반 크기)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RangeTrace")
	FVector BoxHalfSize = FVector(30.f, 60.f, 75.f);

	// 트레이스 거리(앞으로 쏘는 길이)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RangeTrace")
	float TraceDistance = 3000.f;

	// 명중 확률 (0.0~1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RangeTrace")
	float HitProbability = 0.7f;

	
	// 총 쏘기 연출
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	UNiagaraSystem* NiagaraEffect_MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	UNiagaraSystem* NiagaraEffect_ShellEject;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SFX")
	USoundBase* HitSound;

};