#include "System/XVGameMode.h"
#include "System/XVGameState.h"
#include "System/XVGameInstance.h"
#include "Kismet/GameplayStatics.h"
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
	if (CurrentMapName != "BaseLevel")	
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

	Super::StartGame();
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
	Super::SpawnEnemies();
}

void AXVGameMode::OnEnemyKilled()
{
	Super::OnEnemyKilled();
}

void AXVGameMode::OnWaveTriggered()
{
	
	Super::OnWaveTriggered();
}

void AXVGameMode::OnTimeLimitExceeded()
{
	Super::OnTimeLimitExceeded();
}

void AXVGameMode::EndGame(bool bIsClear)
{
	Super::EndGame(bIsClear);
	
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
					UGameplayStatics::OpenLevel(GetWorld(), "BaseLevel");
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



	




