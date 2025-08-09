// UIFollowerComponent.cpp

#include "UIFollowerComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

UUIFollowerComponent::UUIFollowerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UUIFollowerComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!TargetWidget)
    {
        TargetWidget = GetOwner()->FindComponentByClass<UWidgetComponent>();
        if (TargetWidget)
        {
            UE_LOG(LogTemp, Warning, TEXT("UIFollowerComponent: TargetWidget assigned automatically"));
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("UIFollowerComponent: No WidgetComponent found on owner!"));
        }
    }
}

void UUIFollowerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateWidgetLocation();
}

void UUIFollowerComponent::UpdateWidgetLocation()
{
    if (!TargetWidget) return;

    ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
    if (!OwnerCharacter) return;

    // 카메라 위치/방향 가져오기
    APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
    if (!PC) return;

    FVector CamLoc; FRotator CamRot;
    PC->GetPlayerViewPoint(CamLoc, CamRot);

    const FVector CamRight = FRotationMatrix(CamRot).GetUnitAxis(EAxis::Y);

    // 카메라가 캐릭터 왼쪽(음수) / 오른쪽(양수)에 있는지 판정
    const FVector ToCam = (CamLoc - OwnerCharacter->GetActorLocation()).GetSafeNormal();
    const float sideDot = FVector::DotProduct(ToCam, OwnerCharacter->GetActorRightVector());
    const float sideSign = (sideDot < 0.f) ? -1.f : 1.f; // 왼쪽:-1, 오른쪽:+1

    USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
    if (!Mesh) return;

    const FVector StartLocation = Mesh->GetSocketLocation(TEXT("head"));
    const FVector Forward = OwnerCharacter->GetActorForwardVector();
    const FVector EndLocation = StartLocation + Forward * TraceDistance;

    FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(OwnerCharacter);

    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit, StartLocation, EndLocation, ECC_Visibility, Params);

    if (bHit)
    {
        // 벽에 붙이는 경우: 벽의 right 쪽으로 좌/우 오프셋
        const FVector WallRight = FVector::CrossProduct(Hit.ImpactNormal, FVector::UpVector).GetSafeNormal();
        const FVector SideOffset = (sideSign > 0.f ? WallRight : -WallRight) * SideDistance;

        const FVector WidgetLocation = Hit.ImpactPoint + Hit.ImpactNormal * SurfaceOffset + SideOffset;
        const FRotator WidgetRotation = Hit.ImpactNormal.ToOrientationRotator();

        TargetWidget->SetWorldLocationAndRotation(WidgetLocation, WidgetRotation);
    }
    else
    {
        // 평상시: 카메라의 오른쪽/왼쪽 방향으로 치우치게 배치
        const FVector SideOffset = CamRight * sideSign * SideDistance;
        FVector DefaultLocation = StartLocation + Forward * TraceDistance - Forward * 50.f + DefaultOffset + SideOffset;

        TargetWidget->SetWorldLocation(DefaultLocation);

        // 카메라를 바라보게
        const FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(DefaultLocation, CamLoc);
        TargetWidget->SetWorldRotation(LookAt);
    }

#if WITH_EDITOR
    DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Green, false, 0.1f, 0, 1.0f);
#endif
}
