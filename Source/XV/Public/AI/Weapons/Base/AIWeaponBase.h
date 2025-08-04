#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AIWeaponBase.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class XV_API AAIWeaponBase : public AActor
{
	GENERATED_BODY()

public:
	AAIWeaponBase();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "AI")
	TObjectPtr<UBoxComponent> BoxComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "AI")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "AI")
	TObjectPtr<UAudioComponent> WeaponSound;
	

};
