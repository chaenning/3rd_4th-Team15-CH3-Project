#include "Character/XVPlayerController.h"
#include "EnhancedInputSubsystems.h"

AXVPlayerController::AXVPlayerController()
	: InputMappingContext(nullptr),
	  MoveAction(nullptr),
	  JumpAction(nullptr),
	  LookAction(nullptr),
	  SprintAction(nullptr),
	  FireAction(nullptr),
	  SitAction(nullptr),
	  ZoomAction(nullptr),
	  PickUpAction(nullptr),
	  MainWeaponAction(nullptr),
	  SubWeaponAction(nullptr),
	  OpenDoorAction(nullptr),
	  ReloadAction(nullptr)
{
}

void AXVPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		// Local Player에서 EnhancedInputLocalPlayerSubsystem을 획득
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}
