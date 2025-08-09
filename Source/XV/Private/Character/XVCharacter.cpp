#include "Character/XVCharacter.h"
#include "Character/XVPlayerController.h"
#include "Character/XVPlayerAnimInstance.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "BaseGun.h"
#include "World/ElevatorDoor.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AISense_Hearing.h" // AI 총소리 듣기 용입니다.

AXVCharacter::AXVCharacter()
{
	PrimaryActorTick.bCanEverTick = false;		
	
	DefaultCameraLenght = 250.0f;
	ZoomCameraLenght = 100.0f;
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = DefaultCameraLenght;  
	SpringArmComp->bUsePawnControlRotation = true;  
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	NormalSpeed = 600.0f;
	SprintSpeedMultiplier = 1.5f;
	SprintSpeed = NormalSpeed * SprintSpeedMultiplier;

	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	// 메인 무기 == Rifle or Shotgun
	PrimaryWeaponOffset = CreateDefaultSubobject<USceneComponent>(TEXT("PrimaryWeaponOffset"));
	PrimaryWeaponOffset->SetupAttachment(GetMesh(), TEXT("Rifle_Unequipped"));

	PrimaryWeapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("PrimaryWeapon"));
	PrimaryWeapon->SetupAttachment(PrimaryWeaponOffset);
	PrimaryWeapon->SetChildActorClass(BPPrimaryWeapon);

	// 서브 무기 == Pistol
	SubWeaponOffset = CreateDefaultSubobject<USceneComponent>(TEXT("SubWeaponOffset"));
	SubWeaponOffset->SetupAttachment(GetMesh(), TEXT("Pistol_Unequipped"));

	SubWeapon = CreateDefaultSubobject<UChildActorComponent>(TEXT("SubWeapon"));
	SubWeapon->SetupAttachment(SubWeaponOffset);
	SubWeapon->SetChildActorClass(BPSubWeapon);
	
	//SetWeapon(EWeaponType::Pistol); // 시작할 때 들고 있는 무기 = Pistol
	MainWeaponType = EWeaponType::Rifle; // 주 무기 초기화
	SubWeaponType = EWeaponType::Pistol; // 보조 무기 초기화
	
	// 체력 세팅
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;

	//오버랩 이벤트
	Elevator = nullptr;
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AXVCharacter::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AXVCharacter::OnEndOverlap);
}

void AXVCharacter::SetHealth(float Value)
{
	CurrentHealth = FMath::Clamp( Value, 0.0f, MaxHealth);
}

void AXVCharacter::AddHealth(float Value)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + Value, 0.0f, MaxHealth);
}

float AXVCharacter::GetHealth() const
{
	return CurrentHealth;
}

void AXVCharacter::AddDamage(float Value)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Value, 0.0f, MaxHealth);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Damage"));
	// 피격 애니메이션 추가
}

void AXVCharacter::SetWeapon(EWeaponType Weapon)
{ // 일단 타입마다 필요한게 있을 까 싶어 나눴는데 추가 기능 없으면 간략하게 변경해도 될듯
	CurrentWeaponType = Weapon;
	FString WeaponTypeName = StaticEnum<EWeaponType>()->GetNameStringByValue((int64)CurrentWeaponType);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, WeaponTypeName);

	auto anim = Cast<UXVPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	
	switch (CurrentWeaponType)
	{
	case EWeaponType::Pistol:
		// 무기 교체 애니메이션 및 사운드 재생 필요
		SubWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Pistol_Equipped"));
		PrimaryWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Rifle_Unequipped"));

		anim->PlayGunChangeAnim();
		break;
		
	case EWeaponType::Rifle:
		// 무기 교체 애니메이션 및 사운드 재생 필요
		SubWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Pistol_Unequipped"));
		PrimaryWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Rifle_Equipped"));

		anim->PlayGunChangeAnim();
		break;

	case EWeaponType::ShotGun:
		// 무기 교체 애니메이션 및 사운드 재생 필요
		SubWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Pistol_Unequipped"));
		PrimaryWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Rifle_Equipped"));
		break;
	
	default:
		SubWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Pistol_Unequipped"));
		PrimaryWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Rifle_Unequipped"));
		break;
	}
}

EWeaponType AXVCharacter::GetWeapon() const
{
	return CurrentWeaponType;
}

bool AXVCharacter::GetISRun() const
{
	return bIsRun;
}

bool AXVCharacter::GetIsSit() const
{
	return bIsSit;
}

void AXVCharacter::OnWeaponOverlapBegin(ABaseGun* Weapon)
{
	CurrentOverlappingWeapon = Weapon;

	if (Weapon)
	{
		//CurrentWeaponType = Weapon->GetWeaponType();
		FString WeaponTypeName = UEnum::GetValueAsString(Weapon->GetWeaponType());
		UE_LOG(LogTemp, Log, TEXT("Overlapping Weapon Type: %s"), *WeaponTypeName);
	}
}

void AXVCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		UE_LOG(LogTemp, Log, TEXT("Overlap Actor: %s"), *OtherActor->GetName());
		// 엘리베이터 클래스인지 확인
		AElevatorDoor* OverlapElevator = Cast<AElevatorDoor>(OtherActor);
		if (OverlapElevator)
		{
			Elevator = OverlapElevator;
			UE_LOG(LogTemp, Log, TEXT("Elevator: %s"), *Elevator->GetName());
		}
	}
}

void AXVCharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		UE_LOG(LogTemp, Log, TEXT("End Overlap Actor: %s"), *OtherActor->GetName());
		// 엘리베이터 클래스인지 확인
		AElevatorDoor* OverlapElevator = Cast<AElevatorDoor>(OtherActor);
		if (OverlapElevator)
		{			
			Elevator = nullptr;
			if (!Elevator)
			{
				UE_LOG(LogTemp, Log, TEXT("Elevator: null"));
			}
		}
	}
}

void AXVCharacter::OnWeaponOverlapEnd(const ABaseGun* Weapon)
{
	if (CurrentOverlappingWeapon == Weapon)
	{
		CurrentOverlappingWeapon = nullptr;
		//CurrentWeaponType = EWeaponType::None;
	}
}

void AXVCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
	    // IA를 가져오기 위해 현재 소유 중인 Controller를 ASpartaPlayerController로 캐스팅
	    if (AXVPlayerController* PlayerController = Cast<AXVPlayerController>(GetController()))
	    {
	        if (PlayerController->MoveAction)
	        {
	            // IA_Move 액션 키를 "키를 누르고 있는 동안" Move() 호출
	            EnhancedInput->BindAction(
                    PlayerController->MoveAction,
                    ETriggerEvent::Triggered,
                    this,
                    &AXVCharacter::Move
                );
	        }
            
	        if (PlayerController->JumpAction)
	        {
	            // IA_Jump 액션 키를 "키를 누르고 있는 동안" StartJump() 호출
	            EnhancedInput->BindAction(
                    PlayerController->JumpAction,
                    ETriggerEvent::Triggered,
                    this,
                    &AXVCharacter::StartJump
                );
                
	            // IA_Jump 액션 키에서 "손을 뗀 순간" StopJump() 호출
	            EnhancedInput->BindAction(
                    PlayerController->JumpAction,
                    ETriggerEvent::Completed,
                    this,
                    &AXVCharacter::StopJump
                );
	        }
            
	        if (PlayerController->LookAction)
	        {
	            // IA_Look 액션 마우스가 "움직일 때" Look() 호출
	            EnhancedInput->BindAction(
                    PlayerController->LookAction,
                    ETriggerEvent::Triggered,
                    this,
                    &AXVCharacter::Look
                );
	        }
            
	        if (PlayerController->SprintAction)
	        {
	            // IA_Sprint 액션 키를 "누르고 있는 동안" StartSprint() 호출
	            EnhancedInput->BindAction(
                    PlayerController->SprintAction,
                    ETriggerEvent::Triggered, 
                    this, 
                    &AXVCharacter::StartSprint
                );
	            // IA_Sprint 액션 키에서 "손을 뗀 순간" StopSprint() 호출
	            EnhancedInput->BindAction(
                    PlayerController->SprintAction, 
                    ETriggerEvent::Completed, 
                    this, 
                    &AXVCharacter::StopSprint
                );
	        }
	    	
	    	if (PlayerController->FireAction)
	    	{
	    		// IA_Fire 마우스 좌클릭 할 때 Fire() 호출
	    		EnhancedInput->BindAction(
					PlayerController->FireAction,
					ETriggerEvent::Triggered,
					this,
					&AXVCharacter::Fire
				);
	    	}
	    	if (PlayerController->ZoomAction)
	    	{
	    		EnhancedInput->BindAction(
					PlayerController->ZoomAction,
					ETriggerEvent::Triggered,
					this,
					&AXVCharacter::StartZoom
				);

	    		EnhancedInput->BindAction(
					PlayerController->ZoomAction,
					ETriggerEvent::Completed,
					this,
					&AXVCharacter::StopZoom
				);
	    	}
	    	
	    	if (PlayerController->SitAction)
	    	{
	    		EnhancedInput->BindAction(
					PlayerController->SitAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::Sit
				);
	    	}

	    	if (PlayerController->PickUpAction)
	    	{
	    		EnhancedInput->BindAction(
					PlayerController->PickUpAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::PickUpWeapon
				);
	    	}
	    	if (PlayerController->MainWeaponAction)
	    	{
	    		EnhancedInput->BindAction(
					PlayerController->MainWeaponAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::ChangeToMainWeapon
				);
	    	}	    	
	    	if (PlayerController->SubWeaponAction)
	    	{
	    		EnhancedInput->BindAction(
					PlayerController->SubWeaponAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::ChangeToSubWeapon
				);
	    	}
	    	if (PlayerController->OpenDoorAction)
            {
	    		// IA_OpenDoor F키 누른 순간 OpenDoor() 호출
            	EnhancedInput->BindAction(
            		PlayerController->OpenDoorAction,
            		ETriggerEvent::Started,
            		this,
            		&AXVCharacter::OpenDoor
            	);
            }	    	
	    }
	}
}

void AXVCharacter::Move(const FInputActionValue& value)
{
	if (!Controller) return;
	
	const FVector2D MoveInput = value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void AXVCharacter::StartJump(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		Jump();
	}
}

void AXVCharacter::StopJump(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		StopJumping();
	}
}

void AXVCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookInput = value.Get<FVector2D>();
	
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void AXVCharacter::StartSprint(const FInputActionValue& value)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		bIsRun = true;
	}
}

void AXVCharacter::StopSprint(const FInputActionValue& value)
{
	TargetWalkSpeed = NormalSpeed;
	CurrentWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	bIsRun = false;
}

void AXVCharacter::InterpWalkSpeed()
{
	if (!GetCharacterMovement()) return;

	// 보간
	CurrentWalkSpeed = FMath::FInterpTo(CurrentWalkSpeed, TargetWalkSpeed, GetWorld()->GetDeltaSeconds(), InterpSpeed);
	GetCharacterMovement()->MaxWalkSpeed = CurrentWalkSpeed;
	
	// 충분히 가까워지면 보간 종료
	if (FMath::IsNearlyEqual(CurrentWalkSpeed, TargetWalkSpeed, 1.0f))
	{
		GetCharacterMovement()->MaxWalkSpeed = TargetWalkSpeed;
		GetWorld()->GetTimerManager().ClearTimer(WalkSpeedInterpTimerHandle);
		bIsRun = false;
	}
}

void AXVCharacter::Fire(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Fire"));
		auto anim = Cast<UXVPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		anim->PlayAttackAnim();

		ABaseGun* Weapon = Cast<ABaseGun>(PrimaryWeapon->GetChildActor());
		if (Weapon)
		{
			Weapon->FireBullet();
		}

		auto controller = GetWorld()->GetFirstPlayerController();
		controller->PlayerCameraManager->StartCameraShake(CameraShake);

		// AI 소리 듣기용입니다.
		UAISense_Hearing::ReportNoiseEvent
		(
		GetWorld(),
		GetActorLocation(),			 // 소리 발생 위치
		1.0f,						 // 소리 Loudness (1.0은 일반, 더 크면 더 멀리 들림)
		this,						 // 소리 낸 Actor
		0.f,						 // 일정 지연시간
		FName(TEXT("WeaponFire"))	 // 이벤트 식별 태그(선택)
		);
	}
}

void AXVCharacter::Sit(const FInputActionValue& value)
{
	if (!bIsSit)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Sit"));
		bIsSit = true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Stand"));
		bIsSit = false;
	}
}

void AXVCharacter::StartZoom(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		SpringArmComp->TargetArmLength = ZoomCameraLenght;
	}
}

void AXVCharacter::StopZoom(const FInputActionValue& value)
{
	if (!value.Get<bool>())
	{
		SpringArmComp->TargetArmLength = DefaultCameraLenght;
	}
}

void AXVCharacter::PickUpWeapon(const FInputActionValue& value)
{
	if (value.Get<bool>())
	{
		if (CurrentOverlappingWeapon)
		{
			// 메인 총 및 현재 장착 총 변경
			MainWeaponType = CurrentOverlappingWeapon->GetWeaponType();
			CurrentWeaponType = MainWeaponType;
			UE_LOG(LogTemp, Log, TEXT("Picked up Weapon Type: %d"), (uint8)CurrentWeaponType);

			SetWeapon(CurrentWeaponType);
			
			// 무기 액터 파괴
			CurrentOverlappingWeapon->Destroy();
			// 무기 획득 후 오버랩 무기 초기화
			CurrentOverlappingWeapon = nullptr;
		}
	}
}

void AXVCharacter::ChangeToMainWeapon(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("Change To MainWeapon"));
	SetWeapon(MainWeaponType);
}

void AXVCharacter::ChangeToSubWeapon(const FInputActionValue& value)
{
	UE_LOG(LogTemp, Warning, TEXT("Change To SubWeapon"));
	SetWeapon(SubWeaponType);
}

void AXVCharacter::OpenDoor(const FInputActionValue& value)
{
	if (Elevator)
	{
		Elevator->OpenDoor();
	}		
}
