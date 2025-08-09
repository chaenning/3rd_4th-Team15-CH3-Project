#include "Weapon/TestGun.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

ATestGun::ATestGun()
{
    PrimaryActorTick.bCanEverTick = false;
    
    // 기본값 설정
    bAutoFire = true;
    AutoFireRate = 2.0f;
    DebugDrawDuration = 1.0f;
    DebugLineLength = 5000.0f;
    MuzzleSocketName = TEXT("Muzzle");
}

void ATestGun::BeginPlay()
{
    Super::BeginPlay();
}

void ATestGun::FireBullet()
{
    FVector MuzzleLocation;
    FRotator MuzzleRotation;

    if (GunMesh->DoesSocketExist(MuzzleSocketName))
    {
        MuzzleLocation = GunMesh->GetSocketLocation(MuzzleSocketName);
        MuzzleRotation = GunMesh->GetSocketRotation(MuzzleSocketName);
    }
    else
    {
        MuzzleLocation = GetActorLocation();
        MuzzleRotation = GetActorRotation();
    }

    // 1. 애니메이션 재생 (GunAnimInstance)
    if (GunMesh)
    {
        UGunAnimInstance* GunAnim = Cast<UGunAnimInstance>(GunMesh->GetAnimInstance());
        if (GunAnim)
        {
            GunAnim->bIsFiring = true;

            FTimerHandle ResetFireHandle;
            GetWorld()->GetTimerManager().SetTimer(ResetFireHandle, [GunAnim]()
            {
                GunAnim->bIsFiring = false;
            }, 0.2f, false);
        }
    }

    // 2. 머즐 플래시 재생
    if (MuzzleFlashNiagara)
    {
        UNiagaraFunctionLibrary::SpawnSystemAttached(
            MuzzleFlashNiagara,
            GunMesh,
            MuzzleSocketName,
            FVector::ZeroVector,
            FRotator::ZeroRotator,
            EAttachLocation::SnapToTarget,
            true
        );
    }

    // 3. 발사 사운드
    if (FireSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, FireSound, MuzzleLocation);
    }

    // 4. 디버그 라인 트레이스
    FVector ForwardVector = MuzzleRotation.Vector();
    FVector EndLocation = MuzzleLocation + (ForwardVector * DebugLineLength);

    FHitResult Hit;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        MuzzleLocation,
        EndLocation,
        ECC_Visibility,
        QueryParams
    );

    DrawDebugLine(GetWorld(), MuzzleLocation, bHit ? Hit.ImpactPoint : EndLocation, FColor::Red, false, DebugDrawDuration, 0, 1.5f);

    if (bHit)
    {
        DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 10.0f, 12, FColor::Green, false, DebugDrawDuration);
        if (Hit.GetActor())
        {
            FString HitMsg = FString::Printf(TEXT("Hit: %s"), *Hit.GetActor()->GetName());
            GEngine->AddOnScreenDebugMessage(-1, DebugDrawDuration, FColor::Yellow, HitMsg);
        }
    }

    // 5. 물리 탄환 발사
    if (ProjectileClass)
    {
        FActorSpawnParameters Params;
        Params.Owner = this;
        Params.Instigator = GetInstigator();

        AProjectileBullet* Bullet = GetWorld()->SpawnActor<AProjectileBullet>(ProjectileClass, MuzzleLocation, MuzzleRotation, Params);
        if (Bullet)
        {
            Bullet->InitBullet(3000.0f, 25.0f); // 속도, 데미지
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ProjectileClass is NULL!"));
    }
}

void ATestGun::Reload()
{
    
}

FName ATestGun::GetGunType() const
{
    return FName("TestGun");
}