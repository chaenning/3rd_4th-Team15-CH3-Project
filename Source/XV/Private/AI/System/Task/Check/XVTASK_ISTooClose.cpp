#include "XVTASK_ISTooClose.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UXVTASK_ISTooClose::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
	
	// 플레이어가 너무 가까이 있다.
	// 주의 : 이 값이 너무 크면 공격 제대로 안 할 수 있음. 특히 공격 범위와 너무 차이가 나면 안되며.
	// 주의 : 테스트 결과 웬만하면 공격 범위보다 작게 설정해야 하는데 그 값이 최대 100 ~ 180 정도 차이나면 베스트
	if (CheckDistance > RealDistance)
	{
		return EBTNodeResult::Failed;
	}
	
	return EBTNodeResult::Succeeded;
}
