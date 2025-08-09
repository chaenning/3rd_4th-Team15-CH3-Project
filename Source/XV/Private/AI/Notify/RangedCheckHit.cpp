#include "AI/Notify/RangedCheckHit.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AI/Character/Base/XVEnemyBase.h"
#include "Character/XVCharacter.h"
#include "AI/AIComponents/AIStatusComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

void URangedCheckHit::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (!MeshComp) return;
    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;
    AXVEnemyBase* Enemy = Cast<AXVEnemyBase>(Owner);
    if (!Enemy) return;

    FVector Start = Enemy->GetActorLocation() + Enemy->GetActorForwardVector() * 80.0f;
    FVector End = Start + Enemy->GetActorForwardVector() * TraceDistance;

    FRotator Orientation = Enemy->GetActorRotation();

    // 트레이스 박스 시각화용
    DrawDebugBox(Enemy->GetWorld(), Start, BoxHalfSize, Orientation.Quaternion(), FColor::Blue, false, 2.0f, 0, 2.0f);
    DrawDebugBox(Enemy->GetWorld(), End, BoxHalfSize, Orientation.Quaternion(), FColor::Green, false, 2.0f, 0, 2.0f);
    DrawDebugLine(Enemy->GetWorld(), Start, End, FColor::Red, false, 2.0f, 0, 3.0f);

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
        EDrawDebugTrace::ForDuration,
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
            FVector TraceEnd = Character->GetActorLocation();

            // 라인트레이스로 캐릭터까지 중간에 막힌 게 있는지 체크
            FHitResult BlockCheckHit;
            bool bBlocked = Enemy->GetWorld()->LineTraceSingleByChannel(
                BlockCheckHit,
                TraceStart,
                TraceEnd,
                ECollisionChannel::ECC_Visibility, // 벽 체크: 프로젝트에 맞게 채널 지정
                FCollisionQueryParams(FName(TEXT("RangedHitBlock")), false, Enemy)
            );

            // 디버그: 벽에 막히는지 확인
            DrawDebugLine(Enemy->GetWorld(),
                TraceStart,
                TraceEnd,
                bBlocked ? FColor::Red : FColor::Green,
                false, 2.0f, 0, 2.5f);

            // (A) 벽에 막혀있다면(맞는 캐릭터 이전에 막는 액터가 있다면) 명중X
            if (bBlocked && (BlockCheckHit.GetActor() != Character))
            {
                continue; // 넘어감 (다음 캐릭터 Hit 검사)
            }

            // (B) 명중 확률 체크
            float RandomValue = FMath::FRand();
            if (RandomValue < HitProbability)
            {
                float Damage = 10.f;
                if (UAIStatusComponent* Status = Enemy->FindComponentByClass<UAIStatusComponent>())
                    Damage = Status->AttackDamage;
                Character->AddDamage(Damage);
            }
            else
            {
                // 미스 처리, 사운드 등
            }
            return; // 한 명만 판정 후 종료
        }
    }
    // 아무도 맞지 않으면 미스 처리 (사운드 등)
}