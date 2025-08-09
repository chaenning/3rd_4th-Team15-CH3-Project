#include "World/ElevatorDoor.h"
#include "System/XVGameMode.h"
#include "Components/BoxComponent.h"

AElevatorDoor::AElevatorDoor()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	BoxComponent->SetupAttachment(SceneRoot);	
	LeftDoor = CreateDefaultSubobject<UStaticMeshComponent>("LeftDoor");
	LeftDoor->SetupAttachment(SceneRoot);
	RightDoor = CreateDefaultSubobject<UStaticMeshComponent>("RightDoor");
	RightDoor->SetupAttachment(SceneRoot);

	bOpening = false;
	bClosing = false;
	bIsOpen = false;
	bHasClosedOnce = false;
	
	LeftOpenOffset = FVector(-150.f , 0.f, 0.f);
	RightOpenOffset = FVector(150.f , 0.f, 0.f);
	
	PrimaryActorTick.bCanEverTick = true;
}	

void AElevatorDoor::BeginPlay()
{
	Super::BeginPlay();
	
	LeftClosedPos = LeftDoor->GetRelativeLocation();
	RightClosedPos = RightDoor->GetRelativeLocation();
}

void AElevatorDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bOpening || bClosing)
	{
		const float Speed = 1.f;
		LeftDoor->SetRelativeLocation(FMath::VInterpTo(LeftDoor->GetRelativeLocation(), LeftTargetPos, DeltaTime, Speed));
		RightDoor->SetRelativeLocation(FMath::VInterpTo(RightDoor->GetRelativeLocation(), RightTargetPos, DeltaTime, Speed));

		const float Threshold = 1.0f;
		if (FVector::Dist(LeftDoor->GetRelativeLocation(), LeftTargetPos) < Threshold && FVector::Dist(RightDoor->GetRelativeLocation(), RightTargetPos) < Threshold)
		{
			UE_LOG(LogTemp, Warning, TEXT("A"));
			if (bOpening) bOpening = false;
			if (bClosing)
			{
				bClosing = false;
				if (AXVGameMode* GM = Cast<AXVGameMode>(GetWorld()->GetAuthGameMode()))
				{
					GetWorldTimerManager().SetTimer(
					DoorTimer,
					GM,
					&AXVGameMode::StartGame	,
					5.0f,
					false
					);	
				}
			}
		}
	}	
}

void AElevatorDoor::OpenDoor()
{
	UE_LOG(LogTemp, Warning, TEXT("Door opened"));
	if (bIsOpen || bHasClosedOnce) return;

	LeftTargetPos = LeftClosedPos + LeftOpenOffset;
	RightTargetPos = RightClosedPos + RightOpenOffset;
	bOpening = true;
	bIsOpen = true;
}

void AElevatorDoor::CloseDoor()
{
	if (!bIsOpen) return;

	UE_LOG(LogTemp, Warning, TEXT("Door closed"));
	LeftTargetPos = LeftClosedPos;
	RightTargetPos = RightClosedPos;
	bClosing = true;
	bIsOpen = false;
	bHasClosedOnce = true;
}