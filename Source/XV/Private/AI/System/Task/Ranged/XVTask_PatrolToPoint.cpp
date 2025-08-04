#include "AI/System/Task/Ranged/XVTask_PatrolToPoint.h"
#include "AI/Character/Base/XVEnemyBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/DebugTool/DebugTool.h"

UXVTask_PatrolToPoint::UXVTask_PatrolToPoint()
{
	NodeName = TEXT("Set Next Patrol Point");
}

EBTNodeResult::Type UXVTask_PatrolToPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	AXVEnemyBase* MyCharacter = Cast<AXVEnemyBase>(AIController->GetPawn());
	if (!MyCharacter || MyCharacter->PatrolPoints.Num() == 0) return EBTNodeResult::Failed;

	int32 Index = MyCharacter->CurrentPatrolIndex;

	AActor* NextPoint = MyCharacter->PatrolPoints[Index];

	UE_LOG(Log_XV_AI, Warning, TEXT("Set Next Patrol Point %s"), *NextPoint->GetName());
	
	if (!NextPoint) return EBTNodeResult::Failed;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return EBTNodeResult::Failed;

	Blackboard->SetValueAsObject(FName("TargetPoint"), NextPoint);
	
	MyCharacter->CurrentPatrolIndex = (Index + 1) % MyCharacter->PatrolPoints.Num();

	return EBTNodeResult::Succeeded;

}