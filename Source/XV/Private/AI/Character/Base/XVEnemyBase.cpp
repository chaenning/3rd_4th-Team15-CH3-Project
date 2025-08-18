﻿#include "AI/Character/Base/XVEnemyBase.h"

#include "BrainComponent.h"
#include "NavigationSystem.h"
#include "AI/Weapons/Base/AIWeaponBase.h"
#include "AI/AIComponents/AIStatusComponent.h"
#include "AI/System/AIController/Base/XVControllerBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/AIComponents/AIConfigComponent.h"
#include "AI/DebugTool/DebugTool.h"
#include "AssetTypeActions/AssetDefinition_SoundBase.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "System/XVBaseGameMode.h"
#include "Components/CapsuleComponent.h"
#include "Inventory/Data/Item/ItemData.h"
#include "Item/InteractableItem.h"
#include "Perception/AIPerceptionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

AXVEnemyBase::AXVEnemyBase()
	: RotateSpeed(480.f)
	, Acceleration(2048.f)
	, Deceleration(2048.f)
	, BrakingFriction(2.0f)
	, ControllerDesiredRotation(true)
	, OrientRotationToMovement(true)
	, AttackModeSpeed(400.f)
	, AvoidChance(0.1f)
	, DestroyDelayTime(0.5f)
{
	// 컨트롤러 세팅
	AIControllerClass = AXVControllerBase::StaticClass();
	checkf(AIControllerClass != nullptr, TEXT("AIControllerClass is NULL"));

	// AI Config 가져오기
	AIConfigComponent = CreateDefaultSubobject<UAIConfigComponent>(TEXT("AIConfigComponent"));
	checkf(AIConfigComponent != nullptr, TEXT("AIConfigComponent is NULL"));

	// 스테이터스 가져오기
	AIStatusComponent = CreateDefaultSubobject<UAIStatusComponent>(TEXT("AIStatusComponent"));
	checkf(AIStatusComponent != nullptr, TEXT("AIStatusComponent is NULL"));
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AXVEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
	// 세팅 설정
	AIConfigComponent->ConfigSetting();
	
	// MovementComponent 가져오기
	TObjectPtr<UCharacterMovementComponent> MovementComponent = CastChecked<UCharacterMovementComponent>(GetMovementComponent());
	checkf(MovementComponent != nullptr, TEXT("GetMovementComponent() returned NULL"));
	
	// 회전 속도 조정
	MovementComponent->RotationRate = FRotator(0.0f, RotateSpeed, 0.0f); // 부드러운 회전
         
	// 가속/감속 설정
	MovementComponent->MaxAcceleration = Acceleration;			  // 가속도
	MovementComponent->BrakingDecelerationWalking = Deceleration;  // 감속도
	MovementComponent->BrakingFrictionFactor = BrakingFriction;    // 마찰 계수
         
	// 이동 보간 설정
	MovementComponent->bUseControllerDesiredRotation = ControllerDesiredRotation;  // 컨트롤러 방향으로 부드럽게 회전
	MovementComponent->bOrientRotationToMovement = OrientRotationToMovement;       // 이동 방향으로 캐릭터 회전

	// 무기 끼우기
	// TODO : 무기 어떻게 할거임? 기존의 것 가져오던지 아니면 말던지.
	SetWeapon();
	// checkf(AIWeaponBaseClass != nullptr, TEXT("AIWeaponBaseClass is NULL"));

}

void AXVEnemyBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 무기 파괴
	if (AIWeaponBase)
	{
		AIWeaponBase->Destroy();
	}
	
	if (DestroyTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(DestroyTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("DestroyTimerHandle Clear in EndPlay"));
	}

	Super::EndPlay(EndPlayReason);
}

void AXVEnemyBase::Destroyed()
{
	// 무기 파괴
	if (AIWeaponBase)
	{
		AIWeaponBase->Destroy();
	}
	
	if (DestroyTimerHandle.IsValid())
	{
		GetWorldTimerManager().ClearTimer(DestroyTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("DestroyTimerHandle Clear in Destroyed"));
	}
	
	Super::Destroyed();
}

void AXVEnemyBase::SetWeapon()
{
	if (AIWeaponBaseClass)
	{
		// 무기 소환
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		AIWeaponBase = GetWorld()->SpawnActor<AAIWeaponBase>
		(
			AIWeaponBaseClass, 
			GetActorLocation(), 
			GetActorRotation(), 
			SpawnParams
		);

		// 무기를 소켓 등에 Attach하기
		if (AIWeaponBase)
		{
			AIWeaponBase->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName(TEXT("WeaponSocket")));
		}
	}
}

void AXVEnemyBase::GetDamage(float Damage)
{
	if (bIsDead)
	{
		UE_LOG(Log_XV_AI, Warning, TEXT("GetDamage : bIsDead is %d"), bIsDead);
		if (CachedAIController && CachedAIController->BrainComponent)
		{
			CachedAIController->BrainComponent->StopLogic(TEXT("Dead"));
		}

		return;
	}

	if (bIsAvoid)
	{
		UE_LOG(Log_XV_AI, Warning, TEXT("GetDamage : bIsAvoid is %d"), bIsAvoid);
		return;
	}
	
	AIStatusComponent->CurrentHealth();
	AIStatusComponent->D_Gettter(Damage);

	
	
	UE_LOG(Log_XV_AI, Warning, TEXT("GetDamage : %f"),AIStatusComponent->CurrentHealth());
	UE_LOG(Log_XV_AI, Warning, TEXT("GetDamage : GetDamage Start"));
	
	AXVControllerBase* AIController = Cast<AXVControllerBase>(GetController());
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	
	if (true == AIController->AIBlackBoard->GetValueAsBool(TEXT("bIsBoss")))
	{
		OnDamageEventforHUD();
	}
	
	// ▼ 체력이 0 이하로 떨어졌을 때만 사망 처리!
	if (AIStatusComponent->CurrentHealth() <= 0.f)
	{
		DropItem();
		
		if (true == AIController->AIBlackBoard->GetValueAsBool(TEXT("bIsBoss")))
		{
			if (DeathSound)
			{
				GetCharacterMovement()->GravityScale = 0.0f;
				UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
			}
		}
		
		UE_LOG(Log_XV_AI, Warning, TEXT("GetDamage : GetDamage Start2"));
		AIController->AIPerception->SetActive(false);
		
		//
		APawn* Pawn = AIController->GetPawn();
		AXVEnemyBase* AIEnemy = Cast<AXVEnemyBase>(Pawn);
		UCapsuleComponent* Capsule = AIEnemy->GetCapsuleComponent();
		//
		if (Capsule)
		{
			Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			Capsule->SetCanEverAffectNavigation(false);

			// Mesh의 콜리전도 끄기
			if (USkeletalMeshComponent* MeshComp = AIEnemy->GetMesh())
			{
				MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}

			// 모든 MeshComponent의 콜리전 해제(자식까지 포함)
			TArray<UMeshComponent*> MeshComps;
			AIEnemy->GetComponents<UMeshComponent>(MeshComps);

			for (UMeshComponent* Mesh_Comp : MeshComps)
			{
				Mesh_Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}

		}
		//
		
		if(AXVBaseGameMode* BaseGameMode = GetWorld()->GetAuthGameMode<AXVBaseGameMode>())
		{
			UE_LOG(LogTemp, Warning, TEXT("OnEnemyKilled Succeed"));
			BaseGameMode->OnEnemyKilled();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("OnEnemyKilled failed"));
		}
		
		bIsDead = true;

		AIController->AIBlackBoard->SetValueAsBool(TEXT("bIsDead"), true);
		
		UE_LOG(Log_XV_AI, Warning, TEXT("GetDamage : bIsDead is %d"), bIsDead);
		// 컨트롤러 가져오기 (현재 액터에 바운드된 실제 컨트롤러)
		if (AIController)
		{
			// 1. 움직임 멈춤
			AIController->StopMovement();

			// 포커스 멈추기
			AIController->ClearFocus(EAIFocusPriority::Gameplay);
			
			// 2. 비헤이비어 트리(BrainComponent) 정지
			if (AIController->BrainComponent)
			{
				AIController->BrainComponent->StopLogic(TEXT("Dead"));
			}
		}

		UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
		if (MovementComponent)
		{
			MovementComponent->DisableMovement();
			MovementComponent->StopMovementImmediately();
		}

		// 죽는 모션이 있으면 우선 재생
		if (DeathMontage)
		{
			if (AnimInstance)
			{
				// 델리게이트 람다 바인딩
				// FOnMontageEnded DeathMontageEndedDelegate;
				//DeathMontageEndedDelegate.BindLambda([this](UAnimMontage*, bool){DeathTimer();});

				if (true == AIController->AIBlackBoard->GetValueAsBool(TEXT("bIsBoss")))
				{
					// 슬로우 모션 시작
					UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.2f);

					// 3초 후 복구 (타이머 사용)
					FTimerHandle TimerHandle;
					GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
					{
						UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
					}, 3.0f, false);
				}
				
				AnimInstance->Montage_Play(DeathMontage);
				// AnimInstance->Montage_SetEndDelegate(DeathMontageEndedDelegate, DeathMontage);
				//AnimInstance->Montage_SetPlayRate(DeathMontage, 1.0f);
			}
			else
			{
				//DeathTimer();
			}
		}
		else
		{
			//DeathTimer();
		}
		return;
	}
	
	if (false == AIController->AIBlackBoard->GetValueAsBool(TEXT("bIsBoss")))
	{
		// 기본 : 10% 확률로 애니메이션 실행
		if (AIStatusComponent->CurrentHealth() > 10.f && false == bIsAvoid && FMath::FRand() < AvoidChance)
		{
			AIController->AIBlackBoard->SetValueAsBool(TEXT("bIsAvoiding"), true);
		
			UE_LOG(Log_XV_AI, Warning, TEXT("AvoidChance Succeed"));
			ACharacter* PlayerCharacter = Cast<ACharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
			if (PlayerCharacter)
			{
				AIController->BrainComponent->StopLogic(TEXT("Dead"));
				TryRandomAvoid(PlayerCharacter->GetActorLocation());
			}
		}
		else if (AIStatusComponent->CurrentHealth() > 5.f && false == bIsAvoid && FMath::FRand() > AvoidChance)
		{
			UE_LOG(Log_XV_AI, Warning, TEXT("AvoidChance Fail"));
			// 데미지 받기
			AIStatusComponent->Sub_Health(Damage);
		
			// ▼ 맞으면 피격 처리 몽타주 재생 로직
			AIController->GetBrainComponent()->StopLogic(TEXT("일시정지"));
		
			FOnMontageEnded PainMontageEndedDelegate;
			PainMontageEndedDelegate.BindLambda([this](UAnimMontage*, bool){OnDamageEnded();});

			// PainMontages에 하나라도 있으면
			if (PainMontages.Num() > 0)
			{
				int32 Index = FMath::RandRange(0, PainMontages.Num() - 1);
				UAnimMontage* SelectedMontage = PainMontages[Index];

				if (SelectedMontage)
				{
					if (AnimInstance)
					{
						AnimInstance->Montage_Play(SelectedMontage);
						AnimInstance->Montage_SetEndDelegate(PainMontageEndedDelegate, SelectedMontage);
						AnimInstance->Montage_SetPlayRate(SelectedMontage, 1.0f);
					}
				}
			}
			// ▲ 맞으면 피격 처리 몽타주 재생 로직
		}
	}
	else if (true == AIController->AIBlackBoard->GetValueAsBool(TEXT("bIsBoss")))
	{
		// 기본 : 50% 확률로 애니메이션 실행
		if (AIStatusComponent->CurrentHealth() > 10.f && false == bIsAvoid && FMath::FRand() < 0.5f)
		{
			AIController->AIBlackBoard->SetValueAsBool(TEXT("bIsAvoiding"), true);
		
			UE_LOG(Log_XV_AI, Warning, TEXT("AvoidChance Succeed"));
			ACharacter* PlayerCharacter = Cast<ACharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
			if (PlayerCharacter)
			{
				AIController->BrainComponent->StopLogic(TEXT("Dead"));
				TryRandomAvoid(PlayerCharacter->GetActorLocation());
			}
			
		}
		else if (AIStatusComponent->CurrentHealth() > 5.f && false == bIsAvoid && FMath::FRand() > 0.5f)
		{
			UE_LOG(Log_XV_AI, Warning, TEXT("AvoidChance Fail"));
			
			// 데미지 받기
			AIStatusComponent->Sub_Health(Damage);
		
			// ▼ 맞으면 피격 처리 몽타주 재생 로직
			AIController->GetBrainComponent()->StopLogic(TEXT("일시정지"));
		
			FOnMontageEnded PainMontageEndedDelegate;
			PainMontageEndedDelegate.BindLambda([this](UAnimMontage*, bool){OnDamageEnded();});

			// PainMontages에 하나라도 있으면
			if (PainMontages.Num() > 0)
			{
				int32 Index = FMath::RandRange(0, PainMontages.Num() - 1);
				UAnimMontage* SelectedMontage = PainMontages[Index];

				if (SelectedMontage)
				{
					if (AnimInstance)
					{
						AnimInstance->Montage_Play(SelectedMontage);
						AnimInstance->Montage_SetEndDelegate(PainMontageEndedDelegate, SelectedMontage);
						AnimInstance->Montage_SetPlayRate(SelectedMontage, 1.0f);
					}
				}
			}
			// ▲ 맞으면 피격 처리 몽타주 재생 로직
		}
	}
}

void AXVEnemyBase::SetAttackMode()
{
	// MovementComponent 가져오기
	UCharacterMovementComponent* MovementComponent = CastChecked<UCharacterMovementComponent>(GetMovementComponent());
	checkf(MovementComponent != nullptr, TEXT("GetMovementComponent() returned NULL"));

	// 공격모드 속도로 설정
	MovementComponent->MaxWalkSpeed = AttackModeSpeed;
	
} 

void AXVEnemyBase::DeathTimer()
{
	if (IsActorBeingDestroyed())
	{
		return;
	}

	UE_LOG(Log_XV_AI, Warning, TEXT("DeathTimer Start"));
	
	// SetActorHiddenInGame(true);      // 액터 전체 숨김(컴포넌트 관계 없음)
	// SetActorEnableCollision(false);  // 충돌도 끔(안 밟힘)
	// SetActorTickEnabled(false);      // 틱도 끔(CPU 소모 방지)
	//

	GetMesh()->bPauseAnims = true;

	// GetMesh()->SetVisibility(false, true);
	// GetMesh()->SetHiddenInGame(true, true);
	
	for (UActorComponent* Comp : GetComponents())
	{
		UMeshComponent* MeshComp = Cast<UMeshComponent>(Comp);
		if (MeshComp)
			{
			MeshComp->SetVisibility(false, true);
			MeshComp->SetHiddenInGame(true, true);
			UE_LOG(LogTemp, Warning, TEXT("%s: V:%d, H:%d"), *MeshComp->GetName(), MeshComp->IsVisible(), MeshComp->bHiddenInGame);
		}
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		DestroyTimerHandle,
		[this]()
		{
			if (IsValid(this))
			{
				Destroy();
			}
		},
		DestroyDelayTime,
		false
	);

	UE_LOG(Log_XV_AI, Warning, TEXT("DeathTimer End"));
}

void AXVEnemyBase::OnDamageEnded()
{
	AXVControllerBase* AIController = Cast<AXVControllerBase>(GetController());
	if (AIController && AIController->BehaviorTreeAsset)
	{
		AIController->RunBehaviorTree(AIController->BehaviorTreeAsset);
	}

	if (true == AIController->AIBlackBoard->GetValueAsBool(TEXT("bIsBoss")) && FMath::FRand() < 0.5f)
	{
		if (AIStatusComponent->CurrentHealth() <= 0.f)
		{
			return;
		}
		else
		{
			TryRandomPortal();
		}
	}
}

void AXVEnemyBase::TryRandomAvoid(const FVector& PlayerLocation)
{
	UE_LOG(Log_XV_AI, Warning, TEXT("TryRandomAvoid : TryRandomAvoid Start"));
	bIsAvoid = true;

	// 1. 플레이어 바라보는 방향 계산
	FVector ToPlayer = (PlayerLocation - GetActorLocation()).GetSafeNormal();

	// 2. 오른쪽, 왼쪽, 뒤 중에서 랜덤
	int32 Dir = FMath::RandRange(0, 2);

	FVector DodgeDir;
	UAnimMontage* MontageToPlay = nullptr;

	switch (Dir)
	{
	case 0: // 오른쪽
		DodgeDir = FVector::CrossProduct(ToPlayer, FVector::UpVector);
		MontageToPlay = AvoidMontageRight;
		break;
	case 1: // 왼쪽
		DodgeDir = -FVector::CrossProduct(ToPlayer, FVector::UpVector);
		MontageToPlay = AvoidMontageLeft;
		break;
	default: // 뒤
		DodgeDir = -ToPlayer;
		MontageToPlay = AvoidMontageBack;
		break;
	}
	DodgeDir.Normalize();
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	
	// 3. 몽타주 재생 (회피용)
	if (AnimInstance && MontageToPlay)
	{
		AnimInstance->Montage_Play(MontageToPlay);
		FOnMontageEnded AvoidMontageEndedDelegate;
		AvoidMontageEndedDelegate.BindLambda([this](UAnimMontage* Montage, bool bInterrupted){EndAvoid();});
		AnimInstance->Montage_SetEndDelegate(AvoidMontageEndedDelegate, MontageToPlay);
	}

	// 4. 회피 이동 처리 (예: LaunchCharacter로 순간 이동 느낌)
	float DodgeStrength = GetCharacterMovement()->GetMaxSpeed()*100000;
	LaunchCharacter(DodgeDir * DodgeStrength, true, true);
}

void AXVEnemyBase::EndAvoid()
{
	UE_LOG(LogTemp, Warning, TEXT("EndAvoid : EndAvoid Start"));
	
	bIsAvoid = false; // 중단/정상 무관하게 명확히 false 처리

	// AI 재활성화(1초 후 실행) 
	AXVControllerBase* AIController = Cast<AXVControllerBase>(GetController());
	if (AIController && AIController->BehaviorTreeAsset)
	{
		CachedAIController = AIController;
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(
			TimerHandle,
			this,
			&AXVEnemyBase::RunBTWithDelay,
			1.0f, false
		);
	}

	if (true == AIController->AIBlackBoard->GetValueAsBool(TEXT("bIsBoss")) &&  FMath::FRand() < 0.5f)
	{
		if (AIStatusComponent->CurrentHealth() <= 0.f)
		{
			return;
		}
		else
		{
			TryRandomPortal();
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Now bIsAvoid boll is %d"), bIsAvoid);
}



void AXVEnemyBase::RunBTWithDelay()
{
	if (CachedAIController && CachedAIController->BehaviorTreeAsset)
	{
		CachedAIController->RunBehaviorTree(CachedAIController->BehaviorTreeAsset);
		CachedAIController->AIBlackBoard->SetValueAsBool(TEXT("bIsAvoiding"), false);
	}
}

void AXVEnemyBase::TryRandomPortal()
{
	UWorld* World = GetWorld();
	if (!World) return;

	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(World, 0);
	if (!PlayerChar) return;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
	if (!NavSys) return;

	const FVector PlayerLoc = PlayerChar->GetActorLocation();
	const FVector PlayerFwd = PlayerChar->GetActorForwardVector().GetSafeNormal2D();

	// 플레이어 시선 뒤쪽 기준점 설정 및 랜덤 탐색 파라미터
	constexpr float BehindDistance = 1200.f;   // 플레이어 뒤로 떨어질 거리
	constexpr float RandomRadius   = 600.f;    // 기준점 주변 랜덤 반경
	constexpr float MinRadius      = 200.f;    // 기준점과 너무 가까운 점 회피
	constexpr int32 MaxTries       = 20;

	const FVector BasePoint = PlayerLoc - PlayerFwd * BehindDistance;

	FNavLocation OutNavLoc;
	bool bFound = false;

	// 1차: 기준점 주변에서 도달 가능한 무작위 지점
	if (NavSys->GetRandomReachablePointInRadius(BasePoint, RandomRadius, OutNavLoc))
	{
		if (FVector::DistSquared(OutNavLoc.Location, BasePoint) >= FMath::Square(MinRadius))
		{
			bFound = true;
		}
	}

	// 2차: 직접 무작위 후보를 생성하여 네비메시에 투영
	for (int32 i = 0; !bFound && i < MaxTries; ++i)
	{
		const FVector RandDir = FMath::VRand().GetSafeNormal2D();
		const float RandDist = FMath::FRandRange(MinRadius, RandomRadius);
		const FVector Candidate = BasePoint + RandDir * RandDist;

		if (NavSys->ProjectPointToNavigation(Candidate, OutNavLoc))
		{
			bFound = true;
			break;
		}
	}

	// 3차: 기준점 자체를 네비메시에 투영
	if (!bFound && NavSys->ProjectPointToNavigation(BasePoint, OutNavLoc))
	{
		bFound = true;
	}

	if (!bFound) return;

	const FRotator KeepRot = GetActorRotation();
	TeleportTo(OutNavLoc.Location, KeepRot, false, false);
}


void AXVEnemyBase::DropItem()
{
	if (ItemData)
	{
		UWorld* World = GetWorld();
		if (!World) return;

		// Row 이름들 뽑기
		TArray<FName> RowNames = ItemData->GetRowNames();
		if (RowNames.Num() == 0) return;
    
		// 랜덤으로 Row 선택
		int32 RandIdx = FMath::RandRange(0, RowNames.Num() - 1);
		FName SelectedRowName = RowNames[RandIdx];

		// 아이템 데이터에서 정보 가져오기
		if (const FItemData* ItemInfo = ItemData->FindRow<FItemData>(SelectedRowName, TEXT("")))
		{
			if (ItemInfo->ItemClass)
			{
				FVector SpawnLoc = GetActorLocation() + FVector(0, 0, 200);
				AInteractableItem* Item = World->SpawnActor<AInteractableItem>(ItemInfo->ItemClass, SpawnLoc, FRotator::ZeroRotator);
				if (Item)
				{
					if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Item->GetRootComponent()))
					{
						PrimComp->SetSimulatePhysics(true);
						PrimComp->SetEnableGravity(true);
					}
				}
			}
		}
	}
}