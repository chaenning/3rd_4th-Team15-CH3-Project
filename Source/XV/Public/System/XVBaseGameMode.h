#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "XVBaseGameMode.generated.h"

UCLASS()
class XV_API AXVBaseGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	virtual void StartGame();
	virtual void SpawnEnemies() const;
	virtual void OnEnemyKilled();
	virtual void OnWaveTriggered();
	virtual void OnTimeLimitExceeded();
	virtual void EndGame(bool bIsClear);
};
