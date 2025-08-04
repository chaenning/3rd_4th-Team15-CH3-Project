#include "AI/System/Task/Ranged/XVTask_FindSnippingLocation.h"
#include "AI/DebugTool/DebugTool.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UXVTask_FindSnippingLocation::UXVTask_FindSnippingLocation()
{
	NodeName = TEXT("Find Snipping Location");
	SnippingLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UXVTask_FindSnippingLocation, SnippingLocationKey));
	TargetKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UXVTask_FindSnippingLocation, TargetKey), AActor::StaticClass());
}

EBTNodeResult::Type UXVTask_FindSnippingLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(Log_XV_AI, Warning, TEXT("Find Snipping Location1"));
	
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	UE_LOG(Log_XV_AI, Warning, TEXT("Find Snipping Location2"));
	
	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn) return EBTNodeResult::Failed;

	UE_LOG(Log_XV_AI, Warning, TEXT("Find Snipping Location3"));
	
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return EBTNodeResult::Failed;

	UE_LOG(Log_XV_AI, Warning, TEXT("Find Snipping Location4"));
	
	AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!Target) return EBTNodeResult::Failed;

	UE_LOG(Log_XV_AI, Warning, TEXT("Find Snipping Location5"));

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());

	FVector TargetLocation = Target->GetActorLocation();
	
	for (int i = 0; i < 20; ++i)
	{
		UE_LOG(Log_XV_AI, Warning, TEXT("Find Snipping Location : for Loop Start"));
		
		FVector RandomPoint;
		if (NavSys->K2_GetRandomReachablePointInRadius(this, TargetLocation, RandomPoint, SearchRadius))
		{
			UE_LOG(Log_XV_AI, Warning, TEXT("K2_GetRandomReachablePointInRadius"));
			
			float Distance = FVector::Dist(RandomPoint, TargetLocation);
			if (Distance >= MinRange && Distance <= MaxRange)
			{
				UE_LOG(Log_XV_AI, Warning, TEXT("Distance Succeed"));
				
				FHitResult Hit;
				bool bVisible = !GetWorld()->LineTraceSingleByChannel(
					Hit,
					RandomPoint,
					TargetLocation,
					ECC_Visibility);
				if (bVisible)
				{
					Blackboard->SetValueAsVector(SnippingLocationKey.SelectedKeyName, RandomPoint);
					UE_LOG(Log_XV_AI, Warning, TEXT("Find Snipping Location Succeeded"));
					return EBTNodeResult::Succeeded;
				}
				else
				{
					UE_LOG(Log_XV_AI, Warning, TEXT("Line Trace Single By Channel Fail"));
				}
			}
			else
			{
				UE_LOG(Log_XV_AI, Warning, TEXT("Distance fail"));
			}
		}
		else
		{
			UE_LOG(Log_XV_AI, Warning, TEXT("K2_GetRandomReachablePointInRadius Fail"));
		}
	}

	UE_LOG(Log_XV_AI, Warning, TEXT("Find Snipping Location Failed"));
	return EBTNodeResult::Failed;
}