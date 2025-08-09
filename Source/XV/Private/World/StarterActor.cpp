#include "World/StarterActor.h"
#include "Components/BoxComponent.h"
#include "World/ElevatorDoor.h"
#include "System/XVGameInstance.h"
#include "Kismet/GameplayStatics.h"

AStarterActor::AStarterActor()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->SetupAttachment(SceneRoot);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	StaticMeshComponent->SetupAttachment(SceneRoot);
	
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AStarterActor::OnStarterBeginOverlap);
}

void AStarterActor::OnStarterBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,	
		int32 OtherBodyIdx,
		bool bFromSweep,
		const FHitResult& SweepResult
	)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		ActivateStarter(OtherActor);
	}
}
void AStarterActor::ActivateStarter(AActor* Activator)
{
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
			{
				if (XVGI->IsWaiting)
				{
					if (AElevatorDoor* Elevator = Cast<AElevatorDoor>(UGameplayStatics::GetActorOfClass(this, AElevatorDoor::StaticClass())))
					{
						GetWorldTimerManager().SetTimer(
						Delayer,
						Elevator,
						&AElevatorDoor::CloseDoor,
						5.0f,
						false
						);
					}
					Destroy();
				}	
			}
		}
	}
}