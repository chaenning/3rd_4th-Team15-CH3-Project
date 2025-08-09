#pragma once

#include "CoreMinimal.h"
#include "BaseGun.h"
#include "GunAnimInstance.h"
#include "ProjectileBullet.h"
#include "NiagaraFunctionLibrary.h"
#include "TestGun.generated.h"

UCLASS()
class XV_API ATestGun : public ABaseGun
{
    GENERATED_BODY()
    
public:
    ATestGun();

    virtual void FireBullet() override;
    virtual void Reload() override;
    virtual FName GetGunType() const override;

protected:
    virtual void BeginPlay() override;
    
    UPROPERTY(EditAnywhere, Category = "Bullet")
    TSubclassOf<AProjectileBullet> ProjectileClass;
    
    // 자동 발사 관련 변수
    UPROPERTY(EditAnywhere, Category = "AutoFire")
    bool bAutoFire;
    
    UPROPERTY(EditAnywhere, Category = "AutoFire")
    float AutoFireRate;
    
    // 이펙트 관련 변수
    UPROPERTY(EditAnywhere, Category = "Effects")
    UNiagaraSystem* MuzzleFlashNiagara;
    
    UPROPERTY(EditAnywhere, Category = "Effects")
    USoundBase* FireSound;
    
    UPROPERTY(EditAnywhere, Category = "Effects")
    FName MuzzleSocketName;

    // 디버그 설정
    UPROPERTY(EditAnywhere, Category = "Debug")
    float DebugDrawDuration;
    
    UPROPERTY(EditAnywhere, Category = "Debug")
    float DebugLineLength;

private:
    
    // 컨스트럭터에서 초기화할 변수들
    FRotator DefaultRotation;
};