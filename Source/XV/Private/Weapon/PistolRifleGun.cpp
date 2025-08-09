#include "Weapon/PistolRifleGun.h"
#include "Kismet/GameplayStatics.h"

APistolRifleGun::APistolRifleGun()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APistolRifleGun::BeginPlay()
{
	Super::BeginPlay();
}

