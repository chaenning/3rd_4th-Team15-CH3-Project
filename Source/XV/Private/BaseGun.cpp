#include "BaseGun.h"

ABaseGun::ABaseGun()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentWeaponType = EWeaponType::None;
	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	RootComponent = GunMesh;
}

EWeaponType ABaseGun::GetWeaponType()
{
	return CurrentWeaponType;
}

void ABaseGun::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseGun::LoadWeaponData()
{
	if (!WeaponDataTable) return;

	static const FString Context(TEXT("WeaponDataLoad"));
	FWeaponStat* LoadedStat = WeaponDataTable->FindRow<FWeaponStat>(WeaponRowName, Context);

	if (LoadedStat)
	{
		WeaponStat = *LoadedStat;

		// 스켈레톤 메쉬 적용
		if (WeaponStat.WeaponMesh)
		{
			GunMesh->SetSkeletalMesh(WeaponStat.WeaponMesh);
		}

		// 탄약, 데미지 등 설정
		//AmmoCount = WeaponStat.MaxAmmo;
		//FireRate = WeaponStat.FireRate;
		//Damage = WeaponStat.Damage;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponStat not found for row: %s"), *WeaponRowName.ToString());
	}
}


void ABaseGun::OnGunOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ABaseGun::OnGunEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void ABaseGun::FireBullet()
{
}

void ABaseGun::Reload()
{
}

FName ABaseGun::GetGunType() const
{
	return NAME_None;
}

