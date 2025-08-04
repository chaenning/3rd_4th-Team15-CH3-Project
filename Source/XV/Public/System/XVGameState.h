#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "XVGameState.generated.h"

UCLASS()
class XV_API AXVGameState : public AGameState
{
	GENERATED_BODY()

public:
	AXVGameState();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules|Time")
	float TimeLimit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules|Spawn")
	int32 SpawnAllEnemyCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules|Spawn")
	int32 SpawnPatrolEnemyCount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Rules|Wave")
	bool IsWaveTriggered;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Rules|Wave")
	bool CanActiveArrivalPoint;

	int32 SpawnedEnemyCount;
	int32 KilledEnemyCount;

	
};
