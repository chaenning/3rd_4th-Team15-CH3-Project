#include "TestGun.h"
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
    MuzzleSocketName = TEXT("MuzzleSocket");
}

void ATestGun::BeginPlay()
{
    Super::BeginPlay();

    // 자동 발사 시작
    if (bAutoFire)
    {
        GetWorld()->GetTimerManager().SetTimer(
            AutoFireTimerHandle,
            this,
            &ATestGun::AutoFireTimerCallback,
            AutoFireRate,
            true
        );
    }
}

void ATestGun::FireBullet()
{
    // 총구 위치와 회전 계산
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

    // Z축 방향 반전 (180도)
    FVector DownVector = MuzzleRotation.RotateVector(FVector::ForwardVector);
    FVector EndLocation = MuzzleLocation + (DownVector * DebugLineLength);

    // 라인 트레이스 설정
    FHitResult Hit;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    // 라인 트레이스 실행
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        MuzzleLocation,
        EndLocation,
        ECC_Visibility,
        QueryParams
    );

    // 머즐 플래시 이펙트 재생
    if (MuzzleFlashNiagara)
    {
        if (GunMesh->DoesSocketExist(MuzzleSocketName))
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
        else
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                GetWorld(),
                MuzzleFlashNiagara,
                MuzzleLocation,
                MuzzleRotation
            );
        }
    }

    // 발사 사운드 재생
    if (FireSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            this,
            FireSound,
            MuzzleLocation,
            1.0f,
            1.0f,
            0.0f
        );
    }

    // 디버그 라인 그리기
    DrawDebugLine(
        GetWorld(),
        MuzzleLocation,
        bHit ? Hit.ImpactPoint : EndLocation,
        FColor::Red,
        false,
        DebugDrawDuration,
        0,
        2.0f
    );

    // 히트 결과 표시
    if (bHit)
    {
        DrawDebugSphere(
            GetWorld(),
            Hit.ImpactPoint,
            10.0f,
            12,
            FColor::Green,
            false,
            DebugDrawDuration
        );

        if (Hit.GetActor())
        {
            FString HitMessage = FString::Printf(TEXT("Hit: %s"), *Hit.GetActor()->GetName());
            GEngine->AddOnScreenDebugMessage(-1, DebugDrawDuration, FColor::Yellow, HitMessage);
        }
    }
}

void ATestGun::AutoFireTimerCallback()
{
    FireBullet();
}

void ATestGun::Reload()
{
    
}

FName ATestGun::GetGunType() const
{
    return FName("TestGun");
}