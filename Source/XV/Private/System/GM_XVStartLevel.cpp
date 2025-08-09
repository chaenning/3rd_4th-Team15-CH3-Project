#include "System/GM_XVStartLevel.h"
#include "system/XVGameState.h"
#include "System/XVGameInstance.h"
#include "World/SpawnVolume.h"
#include "AI/Character/Base/XVEnemyBase.h"
#include "Kismet/GameplayStatics.h"

AGM_XVStartLevel::AGM_XVStartLevel()
{
	IsOutdoor = true;
}

void AGM_XVStartLevel::BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimer(
			Delayer, 
			this, 
			&AGM_XVStartLevel::StartGame,
			60.0f, 
			false
	  );
}

void AGM_XVStartLevel::StartGame()
{
	Super::StartGame();
}

void AGM_XVStartLevel::SpawnEnemies() const
{
	if (IsOutdoor)
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
					if (Volume->ActorHasTag("Start_Patrol"))
					{
						ValidVolumes.Add(Volume);
					}
				}
			}
		
			int32 EnemyToSpawn = GS->SpawnPatrolEnemyCount;
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
	else
	{
		Super::SpawnEnemies();
	}
	
}

void AGM_XVStartLevel::OnEnemyKilled()
{
	UE_LOG(LogTemp, Warning, TEXT("a"));
	if (IsOutdoor)
	{
		if (AXVGameState* GS = GetGameState<AXVGameState>())
		{
			GS->KilledEnemyCount++;
			if (GS->KilledEnemyCount > 0 && GS->KilledEnemyCount >= GS->SpawnedEnemyCount)
			{
				IsOutdoor = false;
				StartGame();
			}	
		}
	}
	else
	{
		Super::OnEnemyKilled();
	}
}

void AGM_XVStartLevel::OnWaveTriggered()
{
	Super::OnWaveTriggered();
}

void AGM_XVStartLevel::EndGame(bool bIsClear)
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UXVGameInstance* XVGI = Cast<UXVGameInstance>(GI))
		{
			UE_LOG(LogTemp, Warning, TEXT("Level Clear!"));
			XVGI->IsWaiting = true;
			UGameplayStatics::OpenLevel(GetWorld(), "BaseLevel");
		}
	}
}


