#include "AI/System/Task/Move/XVTASK_FindRandomLocation.h"

// 추가됨
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "AIController.h"
#include "AI/DebugTool/DebugTool.h"

UXVTASK_FindRandomLocation::UXVTASK_FindRandomLocation()
{
	NodeName = TEXT("Find Random Location");
	LocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UXVTASK_FindRandomLocation, LocationKey));
}

EBTNodeResult::Type UXVTASK_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// 오너 확인
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	// 폰 존재 확인
	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn) return EBTNodeResult::Failed;

	// 네브 메쉬 확인
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!NavSystem) return EBTNodeResult::Failed;

	// 키 확인
	if (LocationKey.SelectedKeyName.IsNone())
	{
		UE_LOG(Log_XV_AI, Error, TEXT("LocationKey is not set in Behavior Tree!"));
		return EBTNodeResult::Failed;
	}

	FNavLocation RandomLocation;
	bool bFound = NavSystem->GetRandomReachablePointInRadius(MyPawn->GetActorLocation(), SearchRadius, RandomLocation);

	if (bFound)
	{
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsVector(LocationKey.SelectedKeyName, RandomLocation.Location);
			return EBTNodeResult::Succeeded;  
		}
	}

	UE_LOG(Log_XV_AI, Warning, TEXT("Find Random Location Failed")); 
	return EBTNodeResult::Failed;  // 실패
}
