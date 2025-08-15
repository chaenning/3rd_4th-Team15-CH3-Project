// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon/WeaponTypes.h"
#include "Inventory/Data/Armor/ArmorType.h"
#include "Inventory/Data/Armor/ArmorData.h"
#include "XVCharacter.generated.h"


class UBoxComponent;
class AInteractableItem;
class UInventoryComponent;
class AXVDoor;
class USpringArmComponent;
class UCameraComponent;
class AGunBase;
class UUIFollowerComponent;
class AHealthPotionItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, Current, float, Max);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentItemChanged, AInteractableItem*, NewItem);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHealthPotionCountChanged, int32, NewCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentWeaponChanged, AGunBase*, NewWeapon);


UCLASS()
class XV_API AXVCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Weapon")
	FOnCurrentWeaponChanged OnCurrentWeaponChanged;
	
	UFUNCTION(BlueprintPure, Category="Weapon")
	AGunBase* GetCurrentWeapon() const { return CurrentWeaponActor; }
	
	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnHealthChanged OnHealthChanged;
	UFUNCTION(BlueprintPure) float GetHealthPercent() const { return (GetMaxHealth() > 0.f) ? GetHealth() / GetMaxHealth() : 0.f; }
	


	UPROPERTY(BlueprintAssignable) FOnCurrentItemChanged OnCurrentItemChanged;
	UPROPERTY(BlueprintAssignable) FOnHealthPotionCountChanged OnHealthPotionCountChanged;

	UFUNCTION(BlueprintCallable) void SetCurrentItem(AInteractableItem* NewItem);
	UFUNCTION(BlueprintPure)   AInteractableItem* GetCurrentItem() const { return CurrentItem; }
	UFUNCTION(BlueprintPure)   int32 GetHealthPotionCount() const { return HealthPotionCount; }

	void StartUseCurrentItem();
	void StopUseCurrentItem();
	void StartUseShieldItem();
	void SetInventoryItem();
	void ConsumeHealthPotion() { HealthPotionCount = FMath::Max(0, HealthPotionCount-1); OnHealthPotionCountChanged.Broadcast(HealthPotionCount); }


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI", meta=(AllowPrivateAccess="true"))
	UUIFollowerComponent* UIFollowerComp = nullptr;
	virtual void BeginPlay() override;
	
	AXVCharacter();

	//인벤토리 반환
	UInventoryComponent* GetInventoryComp() const;

	//체력 관련 함수들
	UFUNCTION(BlueprintCallable, Category="Health")
	void SetHealth(float Value);
	UFUNCTION(BlueprintCallable, Category="Health")
	void SetMaxHealth(float Value);
	UFUNCTION(BlueprintCallable, Category="Health")
	void AddHealth(float Value);
	UFUNCTION(BlueprintPure, Category="Health")
	float GetHealth() const;
	UFUNCTION(BlueprintPure, Category="Health")
	float GetMaxHealth() const;
	
	void AddDamage(float Value);
	void Die();
	void OnDieAnimationFinished();

	// 속도 관련
	void SetSpeed(float Value);
	
	// 장비 장착
	void SetArmor(const FArmorData& NewArmor, EArmorType Armor);

	void SetWeapon(EWeaponType Weapon);
	void SetCameraShake(TSubclassOf<class UCameraShakeBase> Shake);
	UFUNCTION(BlueprintCallable)
	EWeaponType GetWeapon() const;
	UFUNCTION(BlueprintCallable)
	bool GetISRun() const;
	UFUNCTION(BlueprintCallable)
	bool GetIsSit() const;
	UFUNCTION(BlueprintCallable)
	bool GetIsAim() const;
	UFUNCTION(BlueprintCallable)
	float GetTurnRate() const;
	
	// 현재 장착 무기 타입
	UPROPERTY(BlueprintReadOnly, Category="Weapon")
	EWeaponType CurrentWeaponType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Inventory")
	int32 HealthPotionCount = 0;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TSubclassOf<class UCameraShakeBase> CameraShake;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float DefaultCameraLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float ZoomCameraLength;

	// 인벤토리 관련
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UInteractionComponent* InteractionComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UInventoryComponent* InventoryComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|UI")
	class UWidgetComponent* InteractionWidget;

	// 카메라 앉기 관련 변수들
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	FVector StandCameraOffset; // 서 있을 때 카메라 높이
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	FVector SitCameraOffset; // 로테이션 추가
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float CameraOffsetInterpSpeed = 5.f; // 보간 속도
	FTimerHandle CameraOffsetTimerHandle;
	bool bIsSetCameraOffset = false;
	// 카메라 줌 관련 변수들
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	float ZoomInterpSpeed = 10.f;
	FTimerHandle ZoomTimerHandle;
	bool bIsZooming;
	bool bIsAim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float NormalSpeed; 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SitSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsSit;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bIsRun;	

	// 장비
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Armor")
	UStaticMeshComponent* HelmetMesh;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="Armor")
	UStaticMeshComponent* VestMesh;	
	
	// 주 무기 타입
	UPROPERTY(BlueprintReadOnly, Category="Weapon")
	EWeaponType MainWeaponType;
	// 서브 무기 타입 = Pistol
	UPROPERTY(BlueprintReadOnly, Category="Weapon")
	EWeaponType SubWeaponType;
	
	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* PrimaryWeapon;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* PrimaryWeaponOffset;
	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* SubWeapon;
	UPROPERTY(VisibleAnywhere)
	USceneComponent* SubWeaponOffset;
	UPROPERTY()
	TSubclassOf<AGunBase> BPPrimaryWeapon;
	UPROPERTY()
	TSubclassOf<AGunBase> BPSubWeapon;
	UPROPERTY()	//현재 장착하고 있는 무기 BP
	AGunBase* CurrentWeaponActor; 
	UPROPERTY()
	AGunBase* CurrentOverlappingWeapon;	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlap")
	AXVDoor* Door;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Overlap")
	AInteractableItem* CurrentItem;

	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void Move(const FInputActionValue& Value);
	UFUNCTION()
	void StartJump(const FInputActionValue& Value);
	UFUNCTION()
	void StopJump(const FInputActionValue& Value);
	UFUNCTION()
	void Look(const FInputActionValue& Value);
	UFUNCTION()
	void StartSprint(const FInputActionValue& Value);
	UFUNCTION()
	void StopSprint(const FInputActionValue& Value);
	UFUNCTION()
	void Fire(const FInputActionValue& Value);
	UFUNCTION()
	void Sit(const FInputActionValue& Value);
	void UpdateCameraOffset();
	UFUNCTION()
	void StartZoom(const FInputActionValue& Value);
	UFUNCTION()
	void StopZoom(const FInputActionValue& Value);
	void StopZoomManual();
	UFUNCTION()
	void ChangeLeftZoom(const FInputActionValue& Value);
	UFUNCTION()
	void ChangeRightZoom(const FInputActionValue& Value);
	void SetZoomDirection(bool bIsLookLeft);
	void UpdateZoom();
	UFUNCTION()
	void PickUpWeapon(const FInputActionValue& Value);
	UFUNCTION()
	void ChangeToMainWeapon(const FInputActionValue& Value);
	UFUNCTION()
	void ChangeToSubWeapon(const FInputActionValue& Value);
	UFUNCTION()
	void OpenDoor(const FInputActionValue& Value);
	UFUNCTION()
	void Reload(const FInputActionValue& Value);
	UFUNCTION()
	void Inventory(const FInputActionValue& Value);
	UFUNCTION()
	void ItemInteract(const FInputActionValue& Value);
	
	UBoxComponent* OverlappedBox;
	UFUNCTION()
	void OnBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	// 오버랩 시작 함수
	void OnWeaponOverlapBegin(AGunBase* Weapon);
	// 오버랩 끝 함수
	void OnWeaponOverlapEnd(const AGunBase* Weapon);
	
	
	UPROPERTY(BlueprintReadOnly, Category="State")
	bool bIsDie;

private:
	AInteractableItem* SpawnPotionForUse(FName ItemName);
	void BroadcastHealth();
	
	// 캐릭터 스테이터스
	float CurrentHealth;
	float MaxHealth;
	float TurnRate;
	
	bool bIsLookLeft;
	bool bZoomLookLeft;		
	FTimerHandle DieTimerHandle;
};
