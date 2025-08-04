
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "XVGameInstance.generated.h"

UCLASS()
class XV_API UXVGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UXVGameInstance();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Date")
	int32 CurrentLevelIdx;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Date")
	bool IsWaiting;
};