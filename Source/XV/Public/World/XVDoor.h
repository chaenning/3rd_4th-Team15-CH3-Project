#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "XVDoor.generated.h"

class UBoxComponent;

UCLASS()
class XV_API AXVDoor : public AActor
{
	GENERATED_BODY()
	
public:	
	AXVDoor();
	virtual void BeginPlay() override;
    virtual void Tick( float DeltaSeconds ) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SceneRoot")
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "StaticMesh|Door")
	UStaticMeshComponent* Door;

	FVector ClosedPos;
	FVector OpenOffset;
	FVector TargetPos;
	bool IsOpening;

	void OpenDoor();
	void DestroyDoor();
};
