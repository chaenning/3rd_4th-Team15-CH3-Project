#include "AI/Notify/AI_PauseAnim.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "AI/Character/Base/XVEnemyBase.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"

void UAI_PauseAnim::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (MeshComp)
	{
		MeshComp->bPauseAnims = true;
        
		// 1. Owner 가져오기 (ACharacter 또는 Pawn)
		AActor* OwnerActor = MeshComp->GetOwner();
		if (OwnerActor)
		{
			// 2. Character 형태로 캐스팅 (필요시)
			ACharacter* Character = Cast<ACharacter>(OwnerActor);
			if (Character)
			{
				// 3. 컨트롤러 가져오기
				AController* Controller = Character->GetController();

				// 4. AIController로 캐스팅 (만약 AI컨트롤러 사용 중이면)
				AAIController* AIController = Cast<AAIController>(Controller);
				if (AIController)
				{
					AIController->ClearFocus(EAIFocusPriority::Gameplay);
					
					AIController->BrainComponent->StopLogic(TEXT("Dead"));

					UBehaviorTreeComponent* BTComp = Cast<UBehaviorTreeComponent>(AIController->BrainComponent);
					if (BTComp)
					{
						BTComp->StopTree(EBTStopMode::Safe); // 또는 ForceImmediate
					}
					UCapsuleComponent* Capsule = Character->GetCapsuleComponent();
					if (Capsule)
					{
						Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
						Capsule->SetCanEverAffectNavigation(false);

						// Mesh의 콜리전도 끄기
						if (USkeletalMeshComponent* Mesh = Character->GetMesh())
						{
							Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
						}

						// 모든 MeshComponent의 콜리전 해제(자식까지 포함)
						TArray<UMeshComponent*> MeshComps;
						Character->GetComponents<UMeshComponent>(MeshComps);

						for (UMeshComponent* Mesh_Comp : MeshComps)
						{
							Mesh_Comp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
						}
					}

					// 타이머로 파괴
					AXVEnemyBase* Enemy = Cast<AXVEnemyBase>(OwnerActor);
					if (!Enemy) return;

					if (UWorld* World = Character->GetWorld())
					{
						World->GetTimerManager().SetTimer(Enemy->DestroyTimerHandle,[Character](){if (IsValid(Character)){Character->Destroy();}},DestroyDelayTime,false);
					}
				}
				
			}
		}
	}
}

