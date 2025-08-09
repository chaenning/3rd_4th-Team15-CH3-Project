#include "Weapon/BaseGun.h"

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

		if (WeaponStat.WeaponMesh)
		{
			GunMesh->SetSkeletalMesh(WeaponStat.WeaponMesh);
		}

		// 탄약 및 속성 초기화
		CurrentAmmo = WeaponStat.MaxAmmo;
		bIsReloading = false;

		UE_LOG(LogTemp, Warning, TEXT("Weapon Loaded: %s | Ammo: %d"), *WeaponStat.WeaponName.ToString(), CurrentAmmo);
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
	if (bIsReloading)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot fire - Reloading..."));
		return;
	}

	if (CurrentAmmo <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Out of ammo - Starting reload..."));
		StartReload();
		return;
	}

	// 탄약 감소
	CurrentAmmo--;
	UE_LOG(LogTemp, Warning, TEXT("Fired! Ammo Left: %d"), CurrentAmmo);

	// 실제 발사 로직은 여기 삽입
	// 예: Spawn Projectile, 이펙트, 사운드 등

	// 탄약 소진 시 자동 장전
	if (CurrentAmmo <= 0)
	{
		StartReload();
	}
}

void ABaseGun::StartReload()
{
	if (bIsReloading) return;

	bIsReloading = true;
	UE_LOG(LogTemp, Warning, TEXT("Reloading..."));

	// 장전 시간 예: 2초
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &ABaseGun::FinishReload, 2.0f, false);
}

void ABaseGun::FinishReload()
{
	CurrentAmmo = WeaponStat.MaxAmmo;
	bIsReloading = false;

	UE_LOG(LogTemp, Warning, TEXT("Reload complete! Ammo refilled: %d"), CurrentAmmo);
}


void ABaseGun::Reload()
{
}

FName ABaseGun::GetGunType() const
{
	return NAME_None;
}

