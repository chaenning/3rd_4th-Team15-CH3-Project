#pragma once

#include "CoreMinimal.h"
#include "GunBase.h" // 또는 BaseGun 상속 클래스
#include "PistolRifleGun.generated.h"

UCLASS()
class XV_API APistolRifleGun : public AGunBase
{
	GENERATED_BODY()

public:
	APistolRifleGun();

protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle AutoFireTimerHandle;

	bool bIsFiring = false;
};
