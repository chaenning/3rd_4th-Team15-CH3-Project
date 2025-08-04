#include "AI/AIComponents/AIConfigComponent.h"
#include "AI/System/AIController/Base/XVControllerBase.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionComponent.h"

UAIConfigComponent::UAIConfigComponent()
	: AttackRange(100)
	, AIHearingRange(1000.f)
	, AIHearingSetMaxAge(10.f)
	, AISightRadius(1000.f)
	, AILoseSightRadius(1500.f)
	, AIPeripheralVisionAngleDegrees(180.f)
	, AISightSetMaxAge(10.f)
	, AIbDetectEnemies(true)
	, AIbDetectNeutrals(true)
	, AIbDetectFriendlies(true)
{
}

void UAIConfigComponent::ConfigSetting()
{
	// [1]소유한 폰(캐릭터) 가져오기
	APawn* Owner = Cast<APawn>(GetOwner());
	checkf(Owner != nullptr, TEXT("Owner is NULL"));
	
	// [2]소유한 폰(캐릭터) 에서 AI 컨트롤러 가져오기
	AXVControllerBase* AIController = Cast<AXVControllerBase>(Owner->GetController());
	checkf(AIController != nullptr, TEXT("AIController is NULL"));

	// [3] AI 컨트롤러에서 SightConfig 로 접근해서 값 적용 전 체크
	checkf(AIController->AISightConfig != nullptr, TEXT("AIController->AISightConfig is NULL"));
	
    // SightConfig에 DataAsset 값 적용
	AIController->AISightConfig->SightRadius = AISightRadius;
	AIController->AISightConfig->LoseSightRadius = AILoseSightRadius;
	AIController->AISightConfig->PeripheralVisionAngleDegrees = AIPeripheralVisionAngleDegrees;
	AIController->AISightConfig->SetMaxAge(AISightSetMaxAge);

	// HearingConfig에 DataAsset 값 적용
	AIController->AIHearingConfig->HearingRange = AIHearingRange;
	AIController->AIHearingConfig->SetMaxAge(AIHearingSetMaxAge);

	// 감지 설정 적용 (눈)
	AIController->AISightConfig->DetectionByAffiliation.bDetectEnemies = AIbDetectEnemies;
	AIController->AISightConfig->DetectionByAffiliation.bDetectNeutrals = AIbDetectNeutrals;
	AIController->AISightConfig->DetectionByAffiliation.bDetectFriendlies = AIbDetectFriendlies;

	// 감지 설정 적용 (귀)
	AIController->AISightConfig->DetectionByAffiliation.bDetectEnemies = AIbDetectEnemies;
	AIController->AISightConfig->DetectionByAffiliation.bDetectNeutrals = AIbDetectNeutrals;
	AIController->AISightConfig->DetectionByAffiliation.bDetectFriendlies = AIbDetectFriendlies;

    // 설정한 AI 퍼셉션 업데이트  
	AIController->AIPerception->RequestStimuliListenerUpdate();
	
}



