﻿#include "AI/Notify/RangedCheckHit.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AI/Character/Base/XVEnemyBase.h"
#include "Character/XVCharacter.h"
#include "AI/AIComponents/AIStatusComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "AI/System/AIController/Base/XVControllerBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"

void URangedCheckHit::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (!MeshComp) return;
    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;
    AXVEnemyBase* Enemy = Cast<AXVEnemyBase>(Owner);
    if (!Enemy) return;

    AXVControllerBase* AIController = Cast<AXVControllerBase>(Enemy->GetController());
    bool CheckAvoiding = AIController ? AIController->AIBlackBoard->GetValueAsBool(TEXT("bIsAvoiding")) : false;

    if (CheckAvoiding)
    {
        return;
    }

    // 플레이어 캐릭터를 찾음 (싱글플레이 기준)
    AXVCharacter* PlayerChar = Cast<AXVCharacter>(UGameplayStatics::GetPlayerCharacter(Enemy->GetWorld(), 0));
    if (!PlayerChar) return;

    FVector Start = Enemy->GetActorLocation() + Enemy->GetActorForwardVector() * 80.0f + FVector(0.0f, 0.0f, 100.0f);
    FVector End = PlayerChar->GetActorLocation() + FVector(0.0f, 0.0f, 40.0f);

    FRotator Orientation = (End - Start).Rotation();
    
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    TArray<AActor*> IgnoreActors;
    IgnoreActors.Add(Enemy);

    TArray<FHitResult> OutHits;
    UKismetSystemLibrary::BoxTraceMultiForObjects(
        Enemy->GetWorld(),
        Start, End,
        BoxHalfSize,
        Orientation,
        ObjectTypes,
        false,
        IgnoreActors,
        EDrawDebugTrace::None,
        OutHits,
        true
    );

    for (const FHitResult& Hit : OutHits)
    {
        AXVCharacter* Character = Cast<AXVCharacter>(Hit.GetActor());
        if (Character)
        {
            // Enemy(공격자)에서 Character(맞는 캐릭터)까지 "라인트레이스"로 벽 체크
            FVector TraceStart = Start;
            FVector TraceEnd = End;

            // 라인트레이스로 캐릭터까지 중간에 막힌 게 있는지 체크
            FHitResult BlockCheckHit;
            bool bBlocked = Enemy->GetWorld()->LineTraceSingleByChannel(
                BlockCheckHit,
                TraceStart,
                TraceEnd,
                ECollisionChannel::ECC_Visibility,
                FCollisionQueryParams(FName(TEXT("RangedHitBlock")), false, Enemy)
            );

            if (bBlocked && (BlockCheckHit.GetActor() != Character))
            {
                continue;
            }

            float RandomValue = FMath::FRand();
            if (RandomValue < HitProbability)
            {
                PlayHitEffects(Enemy->GetWorld(), MeshComp);
                float Damage = 10.f;
                if (UAIStatusComponent* Status = Enemy->FindComponentByClass<UAIStatusComponent>())
                    Damage = Status->AttackDamage;
                Character->AddDamage(Damage);
            }
            else
            {
                PlayHitEffects(Enemy->GetWorld(), MeshComp);
            }
            return; // 한 명만 판정 후 종료
        }
    }
    // 아무도 맞지 않으면 미스 처리 (사운드 등)
}


void URangedCheckHit::PlayHitEffects(UWorld* World, USkeletalMeshComponent* Mesh)
{
    if (!World) return;
    
    const FVector MuzzleLoc = Mesh->GetSocketLocation(MuzzleSocketName);
    const FVector ShellLoc = Mesh->GetSocketLocation(ShellEjectSocketName);
    
    if (NiagaraEffect_MuzzleFlash)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            World,
            NiagaraEffect_MuzzleFlash,
            MuzzleLoc
        );
    }
    if (NiagaraEffect_ShellEject)
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            World,
            NiagaraEffect_ShellEject,
            ShellLoc
        );
    }

    if (HitSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            World,
            HitSound,
            MuzzleLoc
        );
    }
}
