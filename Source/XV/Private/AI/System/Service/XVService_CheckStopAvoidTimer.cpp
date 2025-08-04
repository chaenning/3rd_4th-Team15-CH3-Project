#include "AI/System/Service/XVService_CheckStopAvoidTimer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/AIComponents/AIConfigComponent.h"
#include "GameFramework/Pawn.h"

UXVService_CheckStopAvoidTimer::UXVService_CheckStopAvoidTimer()
{
	bNotifyBecomeRelevant = true; // 해당 서비스가 활성화 될 때 알림을 받음
	bNotifyTick = true;           // 매 틱(Tick)마다 TickNode가 호출되도록 함
	AccumulatedTime = 0.0f;       // (미사용 변수, 타이머 누적용)
}

// 매 프레임(틱)마다 실행되며, AI가 특정 조건(회피 유지 시간) 이상일 때 행동 변화 플래그를 블랙보드에 기록
void UXVService_CheckStopAvoidTimer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// 블랙보드와 AIController, 그리고 자신의 Pawn을 안전하게 얻기 / 실패시 즉시 리턴
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !BB) return;

	APawn* MyPawn = AIController->GetPawn();
	if (!MyPawn) return;

	// AI 위치와 플레이어 위치 획득, 거리 계산
	FVector AI_Location = MyPawn->GetActorLocation();
	FVector Player_Location = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	float Distance = FVector::Dist(AI_Location, Player_Location);

	// AI 공격 범위 체크
	UAIConfigComponent* ConfigComp = MyPawn->FindComponentByClass<UAIConfigComponent>();
	float Attackrange = ConfigComp->AttackRange;

	/************** 회피 지속 시간 로직 ***************/

	// 최초로 안전 거리 안에 들어온 경우, 회피 시작 시각 저장 (0이면 아직 저장 안 됨)
	if (StartTimestamp == 0.0f)
	{
		StartTimestamp = GetWorld()->GetTimeSeconds();
	}

	// 플레이어가 회피 임계값 밖으로 벗어난 경우, 회피 타이머와 flag 리셋
	if (Distance > Attackrange)
	{
		StartTimestamp = 0.0f;											// 타이머 초기화 (다시 안쪽으로 들어와야 회피 시간 카운트)
		BB->SetValueAsBool(TEXT("IsStopAvoid"), false); // 블랙보드에서 회피 멈춤 표시 해제
		return;
	}

	// 플레이어가 임계 거리 안에 계속 있으면, 누적 시간 검사
	float CurrentTime = GetWorld()->GetTimeSeconds();
	if (CurrentTime - StartTimestamp >= StopAvoidTime)
	{
		// 일정 시간(StopAvoidTime) 이상 안전 거리 안에 머물렀으면,
		// 블랙보드 Key 'IsStopAvoid'를 true로 변경
		// (실제 행동 트리의 분기는 이 값의 변화를 감지해서 수행)
		BB->SetValueAsBool(TEXT("IsStopAvoid"), true);
	}
	// 만약 시간이 덜 지났으면 아무 일도 하지 않고, 서비스가 계속 Tick을 돌며 조건을 체크함
}
