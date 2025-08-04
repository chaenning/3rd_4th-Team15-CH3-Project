#pragma once

#include "CoreMinimal.h"
#include "Data/EnemySpawnRow.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

class UBoxComponent;

UCLASS()
class XV_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnVolume();

	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* SceneRoot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	UDataTable* EnemyDataTable;

	FEnemySpawnRow* GetRandomEnemy() const;
	FVector GetEnemySpawnPoint() const;
	AActor* SpawnEnemy(TSubclassOf<AActor> EnemyClass);
	AActor* SpawnRandomEnemy();
};
