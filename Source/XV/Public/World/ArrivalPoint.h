#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ArrivalPoint.generated.h"

class UBoxComponent;
UCLASS()
class XV_API AArrivalPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AArrivalPoint();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;
	
	UFUNCTION()
	virtual void OnArrivalPointBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIdx,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
	virtual void ActivateArrivalPoint(AActor* Activator);
};
