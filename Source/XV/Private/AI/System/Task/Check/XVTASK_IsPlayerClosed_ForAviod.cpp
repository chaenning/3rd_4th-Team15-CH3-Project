#include "XVTASK_IsPlayerClosed_ForAviod.h"
#include "AIController.h"
#include "AI/AIComponents/AIConfigComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

EBTNodeResult::Type UXVTASK_IsPlayerClosed_ForAviod::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    // AI 컨트롤러, 소유 폰 체크
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return EBTNodeResult::Failed;

    APawn* MyPawn = AIController->GetPawn();
    if (!MyPawn) return EBTNodeResult::Failed;

    // 블랙보드 컴포넌트 가져오기
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }
    
    // 플레이어 캐릭터 명확하게 얻기
    UWorld* World = MyPawn->GetWorld();
    if (!World) return EBTNodeResult::Failed;
    
    // 거리 계산
    const FVector AI_Location = MyPawn->GetActorLocation();
    const FVector Player_Location = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
    const float Distance = FVector::Dist(AI_Location, Player_Location);

    // AI 공격 범위 체크
    UAIConfigComponent* ConfigComp = MyPawn->FindComponentByClass<UAIConfigComponent>();
    float Attackrange = ConfigComp->AttackRange;

    if (Distance > Attackrange)
    {
        // 플레이어가 너무 멀다
        BlackboardComp->SetValueAsBool(TEXT("IsClosed"),false);
        return EBTNodeResult::Failed;
    }

    // 도망 방향
    const FVector EscapeDir = (AI_Location - Player_Location).GetSafeNormal();
    const FVector TargetLocation = AI_Location + EscapeDir * MoveDistance;

    // 내비게이션에서 이동 지점 보정
    FNavLocation NavResult;
    UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
    
    if (NavSys && NavSys->ProjectPointToNavigation(TargetLocation, NavResult))
    {
        BlackboardComp->SetValueAsVector(TEXT("AvoidLocation"),NavResult.Location);
        BlackboardComp->SetValueAsBool(TEXT("IsClosed"),true);

        return EBTNodeResult::Succeeded;

    }

    return EBTNodeResult::Failed;
}