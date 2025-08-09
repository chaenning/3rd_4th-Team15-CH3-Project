
// UIFollowerComponent.h

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UIFollowerComponent.generated.h"

class UWidgetComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class XV_API UUIFollowerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUIFollowerComponent();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category="UI")
	float SideDistance = 80.f;

	UPROPERTY(EditAnywhere, Category="UI")
	float SurfaceOffset = 10.f;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	
	UPROPERTY(EditAnywhere, Category = "UI")
	float TraceDistance = 100.0f;

	UPROPERTY(EditAnywhere, Category = "UI")
	FVector DefaultOffset = FVector(0.f, 0.f, 0.f);

	UPROPERTY()
	UWidgetComponent* TargetWidget = nullptr;

	void UpdateWidgetLocation();
};


