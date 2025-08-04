#include "XVTASK_CheckSnippingBeforeMove.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UXVTASK_CheckSnippingBeforeMove::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	const float RealDistance = FVector::Dist(AI_Location, Player_Location);
	
	// 플레이어가 너무 멀리 있다.
	// 주의 : 이 값이 IsTooToofar가 차이가 나면 캐릭터안 움직일 수 있음. 
	if (CheckDistance < RealDistance)
	{
		return EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::Succeeded;
}
