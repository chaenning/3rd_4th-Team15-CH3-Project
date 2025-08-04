#include "World/SpawnVolume.h"
#include "Data/EnemySpawnRow.h"
#include "Components/BoxComponent.h"

ASpawnVolume::ASpawnVolume()
{
	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	SpawningBox->SetupAttachment(SceneRoot);

	

	EnemyDataTable = nullptr;
}

void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();
}

FEnemySpawnRow* ASpawnVolume::GetRandomEnemy() const
{
	if (!EnemyDataTable) return nullptr;

	TArray<FEnemySpawnRow*> AllRows;
	static const FString ContextString(TEXT("EnemySpawnContext"));

	EnemyDataTable->GetAllRows(ContextString, AllRows);

	if (AllRows.IsEmpty()) return nullptr;

	float TotalChance = 0.0f;
	for (const FEnemySpawnRow* Row : AllRows)
	{
		if (Row)
		{
			TotalChance += Row->SpawnChance;
		}
	}
	const float RandValue = FMath::FRandRange(0.0f, TotalChance);
	float AccumulateChance = 0.0f;

	for (FEnemySpawnRow* Row : AllRows)
	{
		AccumulateChance += Row->SpawnChance;
		if (RandValue <= AccumulateChance)
		{
			return Row;
		}
	}
	return nullptr;
}

FVector ASpawnVolume::GetEnemySpawnPoint() const	
{
	return SpawningBox->GetComponentLocation();
}

AActor* ASpawnVolume::SpawnEnemy(TSubclassOf<AActor> EnemyClass)
{
	if (!EnemyClass) return nullptr;
	
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
		EnemyClass,	
		GetEnemySpawnPoint(),
		FRotator::ZeroRotator
	);
	return SpawnedActor;
}
AActor* ASpawnVolume::SpawnRandomEnemy()
{
	if (FEnemySpawnRow* SelectedRow = GetRandomEnemy())
	{
		if (UClass* ActualClass = SelectedRow->EnemyClass.Get())
		{
			return SpawnEnemy(ActualClass);
		}
	}
	return nullptr;
}
