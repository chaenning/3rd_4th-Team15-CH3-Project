#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElevatorDoor.generated.h"

class UBoxComponent;

UCLASS()
class XV_API AElevatorDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	AElevatorDoor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SceneRoot")
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh|LeftDoor")
	UStaticMeshComponent* LeftDoor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh|RightDoor")
	UStaticMeshComponent* RightDoor;
	
	FVector LeftClosedPos;
	FVector RightClosedPos;
	
	FVector LeftOpenOffset;
	FVector RightOpenOffset;

	FVector LeftTargetPos;
	FVector RightTargetPos;

	bool bOpening;
	bool bClosing;
	bool bIsOpen;
	bool bHasClosedOnce;

	FTimerHandle DoorTimer;
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;
	void OpenDoor();
	void CloseDoor();

	
};
