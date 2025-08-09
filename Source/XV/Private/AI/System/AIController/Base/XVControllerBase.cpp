#include "AI/System/AIController/Base/XVControllerBase.h"
#include "AI/DebugTool/DebugTool.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AI/Character/Base/XVEnemyBase.h"
#include "System/XVGameMode.h"
#include "System/XVGameState.h"

DEFINE_LOG_CATEGORY(Log_XV_AI);

FGenericTeamId AXVControllerBase::GetGenericTeamId() const
{
	return Super::GetGenericTeamId();
}

ETeamAttitude::Type AXVControllerBase::GetTeamAttitudeTowards(const AActor& Other) const
{
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(&Other);
	
	if(OtherTeamAgent)
	{
		const uint8 OtherTeamID = OtherTeamAgent->GetGenericTeamId().GetId();

		if (OtherTeamID == TeamID)
		{
			return ETeamAttitude::Friendly;
		}
		else
		{
			return ETeamAttitude::Hostile;
		}
	}

	return ETeamAttitude::Neutral;
}

AXVControllerBase::AXVControllerBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// AI Perception 컴포넌트 생성
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	checkf(AIPerception != nullptr, TEXT("AIPerception is NULL"));

	// 감각 설정 생성 (눈)
	AISightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	checkf(AISightConfig != nullptr, TEXT("AISightConfig is NULL"));

	// 감각 설정 생성 (귀)
	AIHearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
	checkf(AIHearingConfig != nullptr, TEXT("AIHearingConfig is NULL"));

	//Blackboard Component 생성
	AIBlackBoard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoard"));
	checkf(AIBlackBoard != nullptr, TEXT("AIBlackBoard is NULL"));
	
	// AI Perception에 감각들 구성
	AIPerception->ConfigureSense(*AISightConfig);
	AIPerception->ConfigureSense(*AIHearingConfig);
	AIPerception->SetDominantSense(AISightConfig->GetSenseImplementation());
}

void AXVControllerBase::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

	// 팀 아이디 설정
	SetGenericTeamId(FGenericTeamId(TeamID)); 
}

void AXVControllerBase::BeginPlay()
{
    Super::BeginPlay();

	// 플레이어 감지시 쓸 함수 바인딩
	AIPerception->OnTargetPerceptionUpdated.AddDynamic(this,&AXVControllerBase::OnTargetInfoUpdated);

	// 비해비어 트리 시작
	checkf(BehaviorTreeAsset != nullptr, TEXT("BehaviorTreeAsset is NULL"));
	RunBehaviorTree(BehaviorTreeAsset);

	// 로그 확인
	LogDataAssetValues();
}

void AXVControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	APawn* ControlledPawn = GetPawn();
	FVector MyLocation = ControlledPawn->GetActorLocation();

	//[1] 본인 위치 실시간 업데이트
	AIBlackBoard->SetValueAsVector(TEXT("MyLocation"), MyLocation);

	//[2] 플레이어 위치 업데이트
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && PC->GetPawn())
	{
		FVector PlayerLocation = PC->GetPawn()->GetActorLocation();
		AIBlackBoard->SetValueAsVector(TEXT("TargetLocation"), PlayerLocation);
	}

	// [3]게임 스테이트 정보 확인하기
	if (AXVGameState* GameState = Cast<AXVGameState>(GetWorld()->GetGameState()))
	{
		// 공격 모드 상태이어야 하는지 확인
		if (true == GameState->IsWaveTriggered)
		{
			// 블랙보드에 공격 모드 세팅 설정
			AIBlackBoard->SetValueAsBool(TEXT("AIIsAttacking"), true);

			//공격 모드 속도로 변경
			if (AXVEnemyBase* Enemy = Cast<AXVEnemyBase>( GetPawn()))
			{
				Enemy->SetAttackMode();
			}
		}
	}
}


void AXVControllerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AIPerception && IsValid(AIPerception))
	{
		// 바인딩 전부 제거
		AIPerception->OnTargetPerceptionUpdated.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void AXVControllerBase::OnTargetInfoUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// 액터 감지 되지 않았거나 블랙보드 없는지 확인
    if (!Actor || !AIBlackBoard)
    {
        return;
    }

    // 플레이어 캐릭터인지 확인
    ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (Actor != PlayerCharacter)
    {
        return;
    }

    // 감지 상태 확인
    const bool bWasSuccessfullySensed = Stimulus.WasSuccessfullySensed();
    
    // 블랙보드 값 업데이트
    AIBlackBoard->SetValueAsObject(TEXT("TargetActor"), Actor);
    AIBlackBoard->SetValueAsBool(TEXT("CanSeeTarget"), bWasSuccessfullySensed);
	AIBlackBoard->SetValueAsBool(TEXT("AIIsAttacking"), true);
	
	//캐릭터 속도 업
	if (AXVEnemyBase* Enemy = Cast<AXVEnemyBase>( GetPawn()))
	{
		Enemy->SetAttackMode();
	}
	
    // 디버그 정보 출력
    const FString StatusText = bWasSuccessfullySensed ? FString::Printf(TEXT("Saw: %s"), *Actor->GetName()) : FString::Printf(TEXT("Lost: %s"), *Actor->GetName());
    DrawDebugString(GetWorld(), Actor->GetActorLocation() + FVector(0, 0, 100), StatusText, nullptr, bWasSuccessfullySensed ? FColor::Green : FColor::Red, 2.0f,true);
	
	// 게임모드 업데이트
	if (AXVGameMode* GameMode = Cast<AXVGameMode>(GetWorld()->GetAuthGameMode()))
	{
		GameMode->OnWaveTriggered();
	}
}

void AXVControllerBase::LogDataAssetValues() const
{
	// 소유 테스트 확인
	
	// Sight Config 적용된 값 확인
	UE_LOG(LogTemp, Warning, TEXT("--- Sight Configuration ---"));
	UE_LOG(LogTemp, Warning, TEXT("Sight Radius: %.2f"), AISightConfig->SightRadius);
	UE_LOG(LogTemp, Warning, TEXT("Lose Sight Radius: %.2f"), AISightConfig->LoseSightRadius);
	UE_LOG(LogTemp, Warning, TEXT("Peripheral Vision Angle: %.2f"), AISightConfig->PeripheralVisionAngleDegrees);

	// Hearing Config 적용된 값 확인
	UE_LOG(LogTemp, Warning, TEXT("--- Hearing Configuration ---"));
	UE_LOG(LogTemp, Warning, TEXT("Hearing Range: %.2f"), AIHearingConfig->HearingRange);

	// 적용된 감지 설정 확인
	UE_LOG(LogTemp, Warning, TEXT("--- Detection Settings ---"));
	UE_LOG(LogTemp, Warning, TEXT("Sight - Detect Enemies: %s"), AISightConfig->DetectionByAffiliation.bDetectEnemies ? TEXT("True") : TEXT("False"));
	UE_LOG(LogTemp, Warning, TEXT("Sight - Detect Neutrals: %s"), AISightConfig->DetectionByAffiliation.bDetectNeutrals ? TEXT("True") : TEXT("False"));
	UE_LOG(LogTemp, Warning, TEXT("Sight - Detect Friendlies: %s"), AISightConfig->DetectionByAffiliation.bDetectFriendlies ? TEXT("True") : TEXT("False"));
    
	UE_LOG(LogTemp, Warning, TEXT("Hearing - Detect Enemies: %s"), AIHearingConfig->DetectionByAffiliation.bDetectEnemies ? TEXT("True") : TEXT("False"));
	UE_LOG(LogTemp, Warning, TEXT("Hearing - Detect Neutrals: %s"), AIHearingConfig->DetectionByAffiliation.bDetectNeutrals ? TEXT("True") : TEXT("False"));
	UE_LOG(LogTemp, Warning, TEXT("Hearing - Detect Friendlies: %s"), AIHearingConfig->DetectionByAffiliation.bDetectFriendlies ? TEXT("True") : TEXT("False"));

	
} // DataAsset 값들을 로그로 출력하는 함수