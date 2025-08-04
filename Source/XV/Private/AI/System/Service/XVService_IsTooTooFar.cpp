#include "AI/System/Service/XVService_IsTooTooFar.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UXVService_IsTooTooFar::UXVService_IsTooTooFar()
{
	bNotifyBecomeRelevant = true;
	bNotifyTick = true;
}

void UXVService_IsTooTooFar::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !BB) return;

	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn) return;

	// 플레이어 얻기 (싱글플레이 기준)
	APlayerController* PlayerController = MyPawn->GetWorld()->GetFirstPlayerController();
	if (!PlayerController) return;

	APawn* PlayerPawn = PlayerController->GetPawn();
	if (!PlayerPawn) return;

	float Distance = FVector::Dist(MyPawn->GetActorLocation(), PlayerPawn->GetActorLocation());

	// 거리 체크 및 블랙보드 값 세팅
	if (Distance > TooTooFarDistance)
	{
		BB->SetValueAsBool(TEXT("IsTooTooFar"), true);
	}
	else
	{
		BB->SetValueAsBool(TEXT("IsTooTooFar"), false);
	}
}
