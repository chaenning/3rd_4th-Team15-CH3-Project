#include "XVTASK_Attackmode.h"

// 추가됨
#include "AIController.h"
#include "AI/AIComponents/AIConfigComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"

UXVTASK_Attackmode::UXVTASK_Attackmode()
{
	NodeName = TEXT("ATTACK_MODE_IsNearPlayer"); 
	MyLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UXVTASK_Attackmode, MyLocationKey));
	PlayerLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UXVTASK_Attackmode, PlayerLocationKey));
}

EBTNodeResult::Type UXVTASK_Attackmode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 오너 확인
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	// 폰 존재 확인
	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn) return EBTNodeResult::Failed;
	
	// 블랙보드 컴포넌트 가져오기
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	// 두 위치 값 가져오기
	FVector MyLocation = BlackboardComp->GetValueAsVector(MyLocationKey.SelectedKeyName);
	FVector PlayerLocation = BlackboardComp->GetValueAsVector(PlayerLocationKey.SelectedKeyName);

	// 두 벡터 사이의 거리 계산
	float Distance = FVector::Distance(MyLocation, PlayerLocation);

	// 공격 가능 범위 가져오기
	UAIConfigComponent* ConfigComp = MyPawn->FindComponentByClass<UAIConfigComponent>();
	float Attackrange = ConfigComp->AttackRange;
	
	// Attackrange보다 작으면 Failed, 아니면 Succeeded 반환
	if(Distance <= Attackrange)
	{
		return EBTNodeResult::Failed;
	}
	
	else if (Distance > Attackrange)
	{
		// 벡터 값 가져오기
		FVector TargetVector = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	
		// 이동 요청 생성
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalLocation(TargetVector);				// 목표 지점
		MoveRequest.SetAcceptanceRadius(50.0f);					// 얼마나 가까이 가야 도착으로 간주할지
		MoveRequest.SetReachTestIncludesAgentRadius(true);		// 콜리전 반경 고려 여부 설정
		MoveRequest.SetUsePathfinding(true);					// 경로 탐색 사용
		MoveRequest.SetAllowPartialPath(true);					// 부분 경로 허용
	
		// 이동 실행
		AIController->MoveTo(MoveRequest);
	}
	
	return EBTNodeResult::Succeeded;
}
