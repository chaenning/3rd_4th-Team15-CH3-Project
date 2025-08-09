
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon/WeaponTypes.h"
#include "XVCharacter.generated.h"

class UUserWidget;
class UWidgetComponent;
class AXVDoor;
class USpringArmComponent;
class UCameraComponent;
class AGunBase;

UCLASS()
class XV_API AXVCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AXVCharacter();

	void SetHealth(float Value);
	void AddHealth(float Value);
	float GetHealth() const;
	void AddDamage(float Value);
	void Die();
	void OnDieAnimationFinished();

	void SetWeapon(EWeaponType Weapon);
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

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="UI")
	void SetTestHUDRef(UUserWidget* HUD);
	
	// 2D HUD
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UUserWidget> TestHUDClass;   // WBP_TestHUD 지정할 슬롯

	UPROPERTY() UUserWidget* TestHUDRef = nullptr;

	// 3D 위젯(블루프린트에 붙어있는 WorldSideUI를 런타임에 찾아서 담을 포인터)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	UWidgetComponent* WorldSideUIRef = nullptr;
	
	// 조준 상태에 따라 보이기/숨기기
	void UpdateUIForAim();
	
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

	UFUNCTION(BlueprintImplementableEvent, Category="State")
	void BP_OnDied();

	UFUNCTION(BlueprintImplementableEvent, Category="State")
	void BP_OnRevived();
	
	UPROPERTY(BlueprintReadOnly, Category="State")
	bool bIsDie;

private:
	
	
	// 캐릭터 스테이터스
	float CurrentHealth;
	float MaxHealth;
	float TurnRate;
	
	bool bIsLookLeft;
	FTimerHandle DieTimerHandle;
};
