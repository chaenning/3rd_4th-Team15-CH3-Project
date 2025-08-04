#include "World/ArrivalPoint.h"
#include "Components/BoxComponent.h"
#include "System/XVGameMode.h"
#include "System/XVGameInstance.h"
#include "System/XVGameState.h"

AArrivalPoint::AArrivalPoint()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->SetupAttachment(SceneRoot);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetupAttachment(SceneRoot);
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AArrivalPoint::OnArrivalPointBeginOverlap);
}

void AArrivalPoint::OnArrivalPointBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,	
		int32 OtherBodyIdx,
		bool bFromSweep,
		const FHitResult& SweepResult
	)
{
	if (AXVGameState* GS = GetWorld() ? GetWorld()->GetGameState<AXVGameState>() : nullptr)
	{
		if (GS->CanActiveArrivalPoint)
		{
			if (OtherActor && OtherActor->ActorHasTag("Player"))
			{	
				ActivateArrivalPoint(OtherActor);
			}
		}
		else UE_LOG(LogTemp, Warning, TEXT("Kill All Enemies!"));
	}
}
void AArrivalPoint::ActivateArrivalPoint(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (AXVGameMode* GM = Cast<AXVGameMode>(GetWorld()->GetAuthGameMode()))
		{
			GM->EndGame(true);
			Destroy();
		}
	}
}