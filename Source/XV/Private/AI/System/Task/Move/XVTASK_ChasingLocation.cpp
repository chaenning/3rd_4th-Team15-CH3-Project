#include "AI/System/Task/Move/XVTASK_ChasingLocation.h"

// 추가됨
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

UXVTASK_ChasingLocation::UXVTASK_ChasingLocation()
{
	bNotifyTick = true;

	NodeName = TEXT("Chasing Location"); 
}

EBTNodeResult::Type UXVTASK_ChasingLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UXVTASK_ChasingLocation::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	
	// 오너 확인
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;

	// 폰 존재 확인
	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn) return;

	// 네브 메쉬 확인
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem) return;

	// 블랙보드 컴포넌트 가져오기
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	// 벡터 값 가져오기
	FVector TargetVector = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	
	// 이동 요청 생성
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(TargetVector);				// 목표 지점
	MoveRequest.SetAcceptanceRadius(30.f);					// 얼마나 가까이 가야 도착으로 간주할지
	MoveRequest.SetReachTestIncludesAgentRadius(true);		// 콜리전 반경 고려 여부 설정
	MoveRequest.SetUsePathfinding(true);					// 경로 탐색 사용
	MoveRequest.SetAllowPartialPath(true);					// 부분 경로 허용
	
	// 이동 실행
	AIController->MoveTo(MoveRequest);
}

