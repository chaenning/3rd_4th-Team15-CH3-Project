#include "AI/Weapons/Base/AIWeaponBase.h"

#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

AAIWeaponBase::AAIWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	checkf(StaticMeshComponent, TEXT("StaticMeshComponent is nullptr"));
	SetRootComponent(StaticMeshComponent);
	
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	checkf(BoxComponent, TEXT("BoxComponent is nullptr"));
	BoxComponent->SetupAttachment(StaticMeshComponent);

	WeaponSound = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	checkf(WeaponSound, TEXT("AudioComponent is nullptr"));
	WeaponSound->SetupAttachment(StaticMeshComponent);
	WeaponSound->bAutoActivate = false;
	
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshComponent->SetSimulatePhysics(false);
	StaticMeshComponent->SetGenerateOverlapEvents(false); 
	StaticMeshComponent->SetNotifyRigidBodyCollision(false);

}
