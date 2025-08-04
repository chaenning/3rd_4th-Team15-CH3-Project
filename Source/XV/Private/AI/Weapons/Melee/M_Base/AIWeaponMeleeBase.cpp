#include "AI/Weapons/Melee/M_Base/AIWeaponMeleeBase.h"

#include "AI/Character/Base/XVEnemyBase.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Character/XVCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AI/AIComponents/AIStatusComponent.h"

AAIWeaponMeleeBase::AAIWeaponMeleeBase()
{
    // 피격 or 미스 사운드 초기화 세팅
    NoHitSound = CreateDefaultSubobject<UAudioComponent>(TEXT("NoHitSound"));
    checkf(NoHitSound, TEXT("NoHitSound is nullptr"));
    NoHitSound->SetupAttachment(StaticMeshComponent);
    NoHitSound->bAutoActivate = false;
}

// 근접 판정 (애니메이션 등에서 직접 호출)
void AAIWeaponMeleeBase::CheckMeleeHit()
{
    // 트레이스 시작/종료 위치(정지 상태라면 동일)
    FVector Start = BoxComponent->GetComponentLocation();
    FVector End = Start;

    // 판정 구체 반경 (공격에 따라 적절히 수정)
    float Radius = 80.f;

    // 대상 ObjectType(여기서는 Pawn만)
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    // 자기 자신은 판정에서 제외
    TArray<AActor*> IgnoreActors;
    IgnoreActors.Add(this);

    // 결과값 저장용 배열
    TArray<FHitResult> OutHits;

    // 구체(스피어) 트레이스 - 여러 명 동시 판정
    UKismetSystemLibrary::SphereTraceMultiForObjects(
        GetWorld(),
        Start, End,
        Radius,
        ObjectTypes,
        false,
        IgnoreActors,
        EDrawDebugTrace::None,
        OutHits,
        true
    );

    // 플레이어 피격 여부 체크
    bool bPlayerHit = false;
    
    for (const FHitResult& Hit : OutHits)
    {
        AActor* HitActor = Hit.GetActor();
        if (HitActor && HitActor->IsA(AXVCharacter::StaticClass()))
        {
            bPlayerHit = true;
            if (WeaponSound)
                WeaponSound->Play();
            
            AXVCharacter* Player = Cast<AXVCharacter>(HitActor);
            AXVEnemyBase* Enemy = CastChecked<AXVEnemyBase>(GetOwner());
            UAIStatusComponent* Component = Enemy->FindComponentByClass<UAIStatusComponent>();

            float Damage = Component->AttackDamage;
            Player->AddDamage(Damage);
            break;
        }
    }

    // 아무 타격이 없으면 미스 사운드 재생
    if (!bPlayerHit)
    {
        if (NoHitSound)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("TODO : Missed! Change Sound!"));
            NoHitSound->Play();
        }
    }
}