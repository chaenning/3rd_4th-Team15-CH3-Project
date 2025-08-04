#include "AI/Notify/MeleeCheckHit.h"
#include "AI/Character/Base/XVEnemyBase.h"
#include "AI/Weapons/Melee/M_Base/AIWeaponMeleeBase.h"

void UMeleeCheckHit::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;

	// MeshComp의 Owner가 AI 캐릭터인지 판단
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	AXVEnemyBase* Enemy = Cast<AXVEnemyBase>(Owner);
	if (!Enemy) return;

	// 무기 오버랩 체크
	if (Enemy->AIWeaponBase)
	{
		CurrentWeapon = Cast<AAIWeaponMeleeBase>(Enemy->AIWeaponBase);
		CurrentWeapon->CheckMeleeHit();
	}
}
