#include "System/XVGameMode.h"
#include "System/XVGameState.h"
#include "System/XVGameInstance.h"
#include "World/ElevatorDoor.h"
#include "Kismet/GameplayStatics.h"
#include "World/SpawnVolume.h"
#include "AI/Character/Base/XVEnemyBase.h"
#include "GameFramework/PlayerController.h"


AXVGameMode::AXVGameMode()
{
	MaxLevel = 3;
}

void AXVGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	FString CurrentMapName = GetWorld()->GetMapName();
	CurrentMapName.RemoveFromStart(GetWorld()->StreamingLevelsPrefix);
	if (CurrentMapName != "LobbyLevel")	
	{
		StartGame();
	}
}

void AXVGameMode::StartGame()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
		{
			if (XVGI->IsWaiting)	
			{
				XVGI->IsWaiting = false;
				
				if (LevelNames.IsValidIndex(XVGI->CurrentLevelIdx))
				{
					UE_LOG(LogTemp, Warning, TEXT("OpenLevel: %s"), *LevelNames[XVGI->CurrentLevelIdx].ToString());
					UGameplayStatics::OpenLevel(GetWorld(), LevelNames[XVGI->CurrentLevelIdx]);
					return;			
				}
			}
		}
	}
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Level Starts!"));
		GS->SpawnedEnemyCount = 0;
		GS->KilledEnemyCount = 0;
	}
	
	SpawnEnemies();

	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		GetWorldTimerManager().SetTimer(
			XVGameTimerHandle,
			this,
			&AXVGameMode::OnTimeLimitExceeded,
			GS->TimeLimit,
			false);
	}
}
	
void AXVGameMode::SpawnEnemies() const
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

void AXVGameMode::OnEnemyKilled()
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

void AXVGameMode::OnWaveTriggered()
{
	
	if (AXVGameState* GS = GetGameState<AXVGameState>())
	{
		GS->CanActiveArrivalPoint = false;
		if (GS->IsWaveTriggered) return;
		
		GS->IsWaveTriggered = true;
		SpawnEnemies();
	}
}

void AXVGameMode::OnTimeLimitExceeded()
{
	EndGame(false);
}

void AXVGameMode::EndGame(bool bIsClear)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->SetPause(true);	
	}
	
	if (bIsClear)
	{
		//Slow Motion
		//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.2);
		
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
			{
				if (XVGI->CurrentLevelIdx <	MaxLevel)
				{
					UE_LOG(LogTemp, Warning, TEXT("Level %d Clear!"), XVGI->CurrentLevelIdx + 1);
					XVGI->CurrentLevelIdx++;
					XVGI->IsWaiting = true;
					UGameplayStatics::OpenLevel(GetWorld(), "LobbyLevel");
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Game Clear!"));
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Over!"));
	}
}



	




