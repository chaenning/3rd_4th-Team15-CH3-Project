#include "System/XVBaseGameMode.h"
#include "System/XVGameState.h"
#include "Kismet/GameplayStatics.h"
#include "World/SpawnVolume.h"
#include "AI/Character/Base/XVEnemyBase.h"
#include "GameFramework/PlayerController.h"

void AXVBaseGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AXVBaseGameMode::StartGame()
{
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Level Starts!"));
		GS->SpawnedEnemyCount = 0;
		GS->KilledEnemyCount = 0;
	}
	
	SpawnEnemies();
}
	
void AXVBaseGameMode::SpawnEnemies() const
{
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
	
	if (FoundVolumes.Num() == 0) return;

	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		TArray<ASpawnVolume*> ValidVolumes;
		for (AActor* Actor : FoundVolumes)
		{
			if (ASpawnVolume* Volume = Cast<ASpawnVolume>(Actor))
			{
				if (!GS->IsWaveTriggered && Volume->ActorHasTag("Patrol"))
				{
					ValidVolumes.Add(Volume);
				}
				else if (GS->IsWaveTriggered && Volume->ActorHasTag("Wave"))
				{
					ValidVolumes.Add(Volume);
				}
			}
		}	
		
		int32 EnemyToSpawn;
		if (!GS->IsWaveTriggered) EnemyToSpawn = GS->SpawnPatrolEnemyCount;
		else EnemyToSpawn = GS->SpawnAllEnemyCount - GS->SpawnPatrolEnemyCount;
		
		const int32 SpawnVolumeCount = ValidVolumes.Num();
		if (SpawnVolumeCount == 0) return;
		for (int32 i = 0; i < EnemyToSpawn; i++)
		{
			ASpawnVolume* SpawnVolume = ValidVolumes[i % SpawnVolumeCount];
			if (!SpawnVolume) continue;
			
			AActor* SpawnActor = SpawnVolume->SpawnRandomEnemy();
			if (SpawnActor && SpawnActor->IsA(AXVEnemyBase::StaticClass()))
			{
				GS->SpawnedEnemyCount++;
			}
		}
	}
}

void AXVBaseGameMode::OnEnemyKilled()
{
	OnWaveTriggered();
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		GS->KilledEnemyCount++;
		if (GS->KilledEnemyCount > 0 && GS->KilledEnemyCount >= GS->SpawnedEnemyCount)
		{
			GS->CanActiveArrivalPoint = true;
		}	
	}
}

void AXVBaseGameMode::OnWaveTriggered()
{
	
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		GS->CanActiveArrivalPoint = false;
		if (GS->IsWaveTriggered) return;
		
		GS->IsWaveTriggered = true;
		SpawnEnemies();
	}
}

void AXVBaseGameMode::OnTimeLimitExceeded()
{
	EndGame(false);
}

void AXVBaseGameMode::EndGame(bool bIsClear)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->SetPause(true);	
	}
}


