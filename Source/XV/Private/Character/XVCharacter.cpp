#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Character/XVCharacter.h"
#include "Character/XVPlayerController.h"
#include "Character/XVPlayerAnimInstance.h"
#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Weapon/GunBase.h"
#include "World/XVDoor.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AISense_Hearing.h" // AI 총소리 듣기 용입니다.
#include "System/XVBaseGameMode.h"

void AXVCharacter::SetTestHUDRef(UUserWidget* HUD)
{
	TestHUDRef = HUD;
	UpdateUIForAim(); // 현재 조준 상태에 맞춰 즉시 가시성 정리
}

AXVCharacter::AXVCharacter()
{
	PrimaryActorTick.bCanEverTick = false;		
	
	DefaultCameraLength = 90.0f; // 기본 스프링암 길이
	ZoomCameraLength = 20.0f; // 줌 스프링암 길이
	StandCameraOffset = FVector(0.f, 0.f, 0.f); // 서 있을 때 카메라 높이
	SitCameraOffset = FVector(0.f, 0.f, -30.f); // 앉아 있을 때 카메라 높이
	
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->TargetArmLength = DefaultCameraLength;  
	SpringArmComp->bUsePawnControlRotation = true;  
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
	CameraComp->bUsePawnControlRotation = false;

	NormalSpeed = 300.0f; // 기본 이동 속도
	SprintSpeed = 500.0f; // 달리기 속도
	SitSpeed = 250.0f; // 앉았을 때 속도

	bIsRun = false;
	bIsSit = false;
	bIsAim = false;
	bIsZooming = false;
	bIsDie = false;

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
	Door = nullptr;
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AXVCharacter::OnBeginOverlap);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &AXVCharacter::OnEndOverlap);
}

void AXVCharacter::SetHealth(float Value)
{
	CurrentHealth = FMath::Clamp( Value, 0.0f, MaxHealth);
}

float AXVCharacter::GetHealth() const
{
	return CurrentHealth;
}

void AXVCharacter::AddHealth(float Value)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + Value, 0.0f, MaxHealth);
}

void AXVCharacter::AddDamage(float Value)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Value, 0.0f, MaxHealth);
	FString Str = FString::Printf(TEXT("%f Damaged, Now Health : %f"), Value, CurrentHealth);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, Str);
	// 피격 애니메이션 추가
	
	if (CurrentHealth <= 0.0f)
	{
		if (!bIsDie)
		{
			Die();
		}
	}
}

void AXVCharacter::Die()
{
	bIsDie = true;
	if (APlayerController* XVPlayerController = Cast<APlayerController>(GetController()))
	{
		UE_LOG(LogTemp, Log, TEXT("PlayerController: %s"), *XVPlayerController->GetName());
		XVPlayerController->DisableInput(XVPlayerController);
	}
	
	if (auto Anim = Cast<UXVPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		Anim->PlayDieAnim();
	}

	GetWorldTimerManager().SetTimer(
	DieTimerHandle,
	this,
	&AXVCharacter::OnDieAnimationFinished,
	2.0f,
	false
	);
	BP_OnDied();
}

void AXVCharacter::OnDieAnimationFinished()
{
	UE_LOG(LogTemp, Warning, TEXT("EndGame"));
	if (AXVBaseGameMode* XVBaseGameMode = Cast<AXVBaseGameMode>(GetWorld()->GetAuthGameMode()))
	{
		XVBaseGameMode->EndGame(false);
	}
}

void AXVCharacter::SetWeapon(EWeaponType Weapon)
{ // 일단 타입마다 필요한게 있을 까 싶어 나눴는데 추가 기능 없으면 간략하게 변경해도 될듯
	if (CurrentWeaponType == Weapon) // 현재 무기와 동일한 무기가 매개변수로 들어온 경우 => 장착 해제
	{
		CurrentWeaponType = EWeaponType::None;
		CurrentWeaponActor = nullptr;
	} 
	else
	{
		CurrentWeaponType = Weapon;
	}

	FString WeaponTypeName = StaticEnum<EWeaponType>()->GetNameStringByValue((int64)CurrentWeaponType);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, WeaponTypeName);

	auto Anim = Cast<UXVPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	
	switch (CurrentWeaponType)
	{
	case EWeaponType::Pistol:
		// 무기 교체 애니메이션 및 사운드 재생 필요
		SubWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Pistol_Equipped"));
		PrimaryWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Rifle_Unequipped"));

		CurrentWeaponActor = Cast<AGunBase>(SubWeapon->GetChildActor());
		Anim->PlayWeaponEquipAnim(CurrentWeaponActor);
		break;
		
	case EWeaponType::Rifle:
		// 무기 교체 애니메이션 및 사운드 재생 필요
		SubWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Pistol_Unequipped"));
		PrimaryWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Rifle_Equipped"));

		CurrentWeaponActor = Cast<AGunBase>(PrimaryWeapon->GetChildActor());
		Anim->PlayWeaponEquipAnim(CurrentWeaponActor);
		break;

	case EWeaponType::ShotGun:
		// 무기 교체 애니메이션 및 사운드 재생 필요
		SubWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Pistol_Unequipped"));
		PrimaryWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Rifle_Equipped"));
		break;
	
	default:
		SubWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Pistol_Unequipped"));
		PrimaryWeaponOffset->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Rifle_Unequipped"));

		CurrentWeaponActor = Cast<AGunBase>(SubWeapon->GetChildActor());
		Anim->PlayWeaponUnequipAnim();
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

bool AXVCharacter::GetIsAim() const
{
	return bIsAim;
}

float AXVCharacter::GetTurnRate() const
{
	return TurnRate;
}

void AXVCharacter::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		//UE_LOG(LogTemp, Log, TEXT("Overlap Actor: %s"), *OtherActor->GetName());
		// 엘리베이터 클래스인지 확인
		if (AXVDoor* OverlapDoor = Cast<AXVDoor>(OtherActor))
		{
			Door = OverlapDoor;
			UE_LOG(LogTemp, Log, TEXT("Door: %s"), *Door->GetName());
		}
	}
}

void AXVCharacter::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		//UE_LOG(LogTemp, Log, TEXT("End Overlap Actor: %s"), *OtherActor->GetName());
		// 엘리베이터 클래스인지 확인
		if (Door == Cast<AXVDoor>(OtherActor))
		{			
			Door = nullptr;
			if (!Door)
			{
				UE_LOG(LogTemp, Log, TEXT("Door: null"));
			}
		}
	}
}

void AXVCharacter::OnWeaponOverlapBegin(AGunBase* Weapon)
{
	CurrentOverlappingWeapon = Weapon;

	if (Weapon)
	{
		//CurrentWeaponType = Weapon->GetWeaponType();
		//FString WeaponTypeName = UEnum::GetValueAsString(Weapon->GetWeaponType());
		//UE_LOG(LogTemp, Log, TEXT("Overlapping Weapon Type: %s"), *WeaponTypeName);
	}
}

void AXVCharacter::OnWeaponOverlapEnd(const AGunBase* Weapon)
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
	    		// IA_Zoom 마우스 우클릭 할 때 StartZoom() 호출
	    		EnhancedInput->BindAction(
					PlayerController->ZoomAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::StartZoom
				);
	    		// IA_Zoom 마우스 우클릭 끝날 때 StopZoom() 호출
	    		EnhancedInput->BindAction(
					PlayerController->ZoomAction,
					ETriggerEvent::Completed,
					this,
					&AXVCharacter::StopZoom
				);
	    	}
	    	
	    	if (PlayerController->SitAction)
	    	{
	    		// IA_Sit 컨트롤 누를 때 Sit() 호출
	    		EnhancedInput->BindAction(
					PlayerController->SitAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::Sit
				);
	    	}

	    	if (PlayerController->PickUpAction)
	    	{
	    		// IA_PickUp E 누를 때 PickUpWeapon() 호출
	    		EnhancedInput->BindAction(
					PlayerController->PickUpAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::PickUpWeapon
				);
	    	}
	    	if (PlayerController->MainWeaponAction)
	    	{
	    		// IA_MainWeapon 1 누를 때 ChangeToMainWeapon() 호출
	    		EnhancedInput->BindAction(
					PlayerController->MainWeaponAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::ChangeToMainWeapon
				);
	    	}	    	
	    	if (PlayerController->SubWeaponAction)
	    	{
	    		// IA_SubWeapon 2 누를 때 ChangeToSubWeapon() 호출
	    		EnhancedInput->BindAction(
					PlayerController->SubWeaponAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::ChangeToSubWeapon
				);
	    	}
	    	if (PlayerController->OpenDoorAction)
            {
	    		// IA_OpenDoor F키 누를 때 OpenDoor() 호출
            	EnhancedInput->BindAction(
            		PlayerController->OpenDoorAction,
            		ETriggerEvent::Started,
            		this,
            		&AXVCharacter::OpenDoor
            	);
            }
	    	if (PlayerController->ReloadAction)
	    	{
	    		// IA_Reload R키 누를 때 Reload() 호출
	    		EnhancedInput->BindAction(
					PlayerController->ReloadAction,
					ETriggerEvent::Started,
					this,
					&AXVCharacter::Reload
				);
	    	}	    	
	    }
	}
}

void AXVCharacter::Move(const FInputActionValue& Value)
{
	if (bIsDie) return;
    if (!Controller) return;
    
    const FVector2D MoveInput = Value.Get<FVector2D>();

    if (!FMath::IsNearlyZero(MoveInput.X))
    {
    	FRotator ControlRotation = Controller->GetControlRotation();
    	FRotator YawRotation(0, ControlRotation.Yaw, 0); // Pitch, Roll 제거 (수평 회전만)
    	FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    	
       AddMovementInput(ForwardDirection, MoveInput.X);
    }

    if (!FMath::IsNearlyZero(MoveInput.Y))
    {
       FRotator ControlRotation = Controller->GetControlRotation();
       FRotator YawRotation(0, ControlRotation.Yaw, 0);
       FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
       
       AddMovementInput(RightDirection, MoveInput.Y);
    }
}

void AXVCharacter::StartJump(const FInputActionValue& Value)
{
	if (bIsDie) return;
	if (Value.Get<bool>() && !bIsSit)
	{
		Jump();
	}
}

void AXVCharacter::StopJump(const FInputActionValue& Value)
{
	if (bIsDie) return;
	if (!Value.Get<bool>())
	{
		StopJumping();
	}
}

void AXVCharacter::Look(const FInputActionValue& Value)
{
	if (bIsDie) return;
	FVector2D LookInput = Value.Get<FVector2D>();
	
	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
	if (LookInput.X > 0.f)
	{
		// 오른쪽으로 회전 중
		bIsLookLeft = false;
	}
	else if (LookInput.X < 0.f)
	{
		// 왼쪽으로 회전 중
		bIsLookLeft = true;
	}
	TurnRate = FMath::Clamp(LookInput.X, -1 ,1);
}

void AXVCharacter::StartSprint(const FInputActionValue& Value)
{	
	if (bIsDie) return;
	if (CurrentWeaponActor)
	{
		if (CurrentWeaponActor->IsReloading()) return;
	}
	
	if (GetCharacterMovement() && !bIsSit)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		bIsRun = true;
	}
}

void AXVCharacter::StopSprint(const FInputActionValue& Value)
{
	if (bIsDie) return;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
	bIsRun = false;
}

void AXVCharacter::Fire(const FInputActionValue& Value)
{
	if (bIsDie) return;
	if (Value.Get<bool>() && CurrentWeaponType != EWeaponType::None && !bIsRun) // 빈 손 일 때, 달릴 때 발사 금지
	{
		auto Anim = Cast<UXVPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		if (Anim)
		{
			Anim->PlayAttackAnim(CurrentWeaponActor);
		}

		if (CurrentWeaponActor)
		{
			CurrentWeaponActor->FireBullet();
		}

		auto XVController = GetWorld()->GetFirstPlayerController();
		XVController->PlayerCameraManager->StartCameraShake(CameraShake);

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

void AXVCharacter::Sit(const FInputActionValue& Value)
{
	if (bIsDie) return;
	if (GetCharacterMovement())
	{		
		if (!bIsSit)
		{		
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Sit"));
			GetCharacterMovement()->MaxWalkSpeed = SitSpeed;
			bIsRun = false;
			bIsSit = true;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Stand"));
			GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
			bIsSit = false;
		}
		if (!bIsSetCameraOffset)
		{
			bIsSetCameraOffset = true;
			GetWorld()->GetTimerManager().SetTimer(
				CameraOffsetTimerHandle,
				this,
				&AXVCharacter::UpdateCameraOffset,
				0.01f,
				true
			);
		}
	}	
}

void AXVCharacter::UpdateCameraOffset()
{
	FVector TargetOffset = bIsSit ? SitCameraOffset : StandCameraOffset;
	FVector CurrentOffset = SpringArmComp->SocketOffset;

	FVector NewOffset = FMath::VInterpTo(CurrentOffset, TargetOffset, GetWorld()->GetDeltaSeconds(), CameraOffsetInterpSpeed);
	SpringArmComp->SocketOffset = NewOffset;

	if (FVector::Dist(NewOffset, TargetOffset) < 1.f)
	{
		SpringArmComp->SocketOffset = TargetOffset;
		GetWorld()->GetTimerManager().ClearTimer(CameraOffsetTimerHandle);
		bIsSetCameraOffset = false;
	}
}

void AXVCharacter::StartZoom(const FInputActionValue& Value)
{
		bIsAim = true;
	    UpdateUIForAim();
		UE_LOG(LogTemp, Log, TEXT("Is Look Left %s"), bIsLookLeft ? TEXT("True") : TEXT("False"));

		if (!bIsZooming)
		{
			bIsZooming = true;
			GetWorld()->GetTimerManager().SetTimer(
				ZoomTimerHandle,
				this,
				&AXVCharacter::UpdateZoom,
				0.01f,
				true
			);
		}
}

void AXVCharacter::StopZoom(const FInputActionValue& Value)
{
	if (!Value.Get<bool>())
	{
		bIsAim = false;
		UpdateUIForAim();
		if (!bIsZooming)
		{
			bIsZooming = true;
			GetWorld()->GetTimerManager().SetTimer(
				ZoomTimerHandle,
				this,
				&AXVCharacter::UpdateZoom,
				0.01f,
				true
			);
		}
	}
}

void AXVCharacter::UpdateZoom()
{
	// 줌 거리 보간
	float TargetLength = bIsAim ? ZoomCameraLength : DefaultCameraLength;
	float CurrentLength = SpringArmComp->TargetArmLength;

	float NewLength = FMath::FInterpTo(CurrentLength, TargetLength, GetWorld()->GetDeltaSeconds(), ZoomInterpSpeed);
	SpringArmComp->TargetArmLength = NewLength;

	// 카메라 위치 보간 조건에 따라 설정
	FVector TargetLocation;

	if (!bIsAim) // 조준 중이 아니면 카메라 원래 대로
	{
		TargetLocation = FVector::ZeroVector;
	}
	else // 현재 바라보고 있는 방향에 따라 카메라 위치 이동
	{
		TargetLocation = bIsLookLeft ? FVector(0.f, -90.f, 0.f) : FVector(0.f, 40.f, 0.f);
	}

	FVector CurrentLocation = CameraComp->GetRelativeLocation();
	FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, GetWorld()->GetDeltaSeconds(), ZoomInterpSpeed);
	CameraComp->SetRelativeLocation(NewLocation);

	// 줌 및 위치 전환 완료 조건
	bool bZoomDone = FMath::Abs(NewLength - TargetLength) < 1.f;
	bool bLocationDone = FVector::Dist(NewLocation, TargetLocation) < 1.f;

	if (bZoomDone && bLocationDone)
	{
		SpringArmComp->TargetArmLength = TargetLength;
		CameraComp->SetRelativeLocation(TargetLocation);

		GetWorld()->GetTimerManager().ClearTimer(ZoomTimerHandle);
		bIsZooming = false;
	}
}

void AXVCharacter::PickUpWeapon(const FInputActionValue& Value)
{
	if (bIsDie) return;
	if (Value.Get<bool>())
	{
		if (CurrentOverlappingWeapon)
		{
			UE_LOG(LogTemp, Log, TEXT("PickUp"));			
		}
	}
}

void AXVCharacter::ChangeToMainWeapon(const FInputActionValue& Value)
{
	if (bIsDie) return;
	UE_LOG(LogTemp, Warning, TEXT("Change To MainWeapon"));
	SetWeapon(MainWeaponType);
}

void AXVCharacter::ChangeToSubWeapon(const FInputActionValue& Value)
{
	if (bIsDie) return;
	UE_LOG(LogTemp, Warning, TEXT("Change To SubWeapon"));
	SetWeapon(SubWeaponType);
}

void AXVCharacter::OpenDoor(const FInputActionValue& Value)
{
	if (bIsDie) return;
	if (Door)
	{
		Door->OpenDoor();
	}		
}

void AXVCharacter::Reload(const FInputActionValue& Value)
{
	if (bIsDie || bIsRun) return;
	if (CurrentWeaponType != EWeaponType::None && !CurrentWeaponActor->IsReloading())
	{
		auto Anim = Cast<UXVPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		if (Anim)
		{
			Anim->PlayReloadAnim(CurrentWeaponActor);
		}
		CurrentWeaponActor->Reload();
	}
}
void AXVCharacter::BeginPlay()
{
	Super::BeginPlay();

	TArray<UWidgetComponent*> Widgets;
	GetComponents<UWidgetComponent>(Widgets);
	for (auto* W : Widgets)
	{
		if (W && (W->ComponentHasTag(TEXT("WorldSideUI")) || W->GetName().Equals(TEXT("WorldSideUI"))))
		{
			WorldSideUIRef = W;
			break;
		}
	}
	UpdateUIForAim();
}
void AXVCharacter::UpdateUIForAim()
{
	UE_LOG(LogTemp, Warning, TEXT("Aim=%d  HUD=%p  3D=%p"),
		bIsAim ? 1 : 0, TestHUDRef, WorldSideUIRef);

	// 3D 위젯은 컴포넌트 가시성으로 토글
	if (WorldSideUIRef)
	{
		WorldSideUIRef->SetHiddenInGame(!bIsAim);   // 조준=true면 보이기
	}

	// 2D HUD는 위젯 내부 애니메이션 이벤트 호출
	if (TestHUDRef)
	{
		static const FName FuncName(TEXT("SetAimState")); // UMG에 만든 이벤트 이름과 같아야 함
		if (UFunction* Fn = TestHUDRef->FindFunction(FuncName))
		{
			struct FParams { bool bAiming; };
			FParams P{ bIsAim };
			TestHUDRef->ProcessEvent(Fn, &P);  // ✅ UMG 애니 실행(정방향/역재생)
		}
		else
		{
			// (백업) 이벤트가 없으면 최소한 보이기/숨기기만 처리
			TestHUDRef->SetVisibility(bIsAim ? ESlateVisibility::Collapsed
											 : ESlateVisibility::HitTestInvisible);
		}
	}
}