#include "Character/XVPlayerAnimInstance.h"

void UXVPlayerAnimInstance::PlayAttackAnim()
{
	Montage_Play(AttackAnimMontage);
}

void UXVPlayerAnimInstance::PlayGunChangeAnim()
{
	Montage_Play(ChangeAnimMontage);
}
