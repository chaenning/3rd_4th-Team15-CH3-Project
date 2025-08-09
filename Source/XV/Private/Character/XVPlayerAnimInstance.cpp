#include "Character/XVPlayerAnimInstance.h"
#include "Weapon/GunBase.h"

void UXVPlayerAnimInstance::PlayAttackAnim(AGunBase* Weapon)
{
	if (Weapon != nullptr)
	{
		AttackAnimMontage = Weapon->GetFireMontage();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon is Nullptr"));
	}
	Montage_Play(AttackAnimMontage);
}

void UXVPlayerAnimInstance::PlayWeaponEquipAnim(AGunBase* Weapon)
{
	if (Weapon != nullptr)
	{
		EquipAnimMontage = Weapon->GetEquipMontage();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon is Nullptr"));
	}
	Montage_Play(EquipAnimMontage);
}

void UXVPlayerAnimInstance::PlayWeaponUnequipAnim()
{
	UE_LOG(LogTemp, Error, TEXT("Unequip"));
	Montage_Play(UnequipAnimMontage);
}

void UXVPlayerAnimInstance::PlayReloadAnim(AGunBase* Weapon)
{
	if (Weapon != nullptr)
	{
		ReloadAnimMontage = Weapon->GetReloadMontage();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon is Nullptr"));
	}
	Montage_Play(ReloadAnimMontage);
}

void UXVPlayerAnimInstance::PlayDieAnim()
{
	Montage_Play(DieAnimMontage);
}
