#include "System/XVGameState.h"

AXVGameState::AXVGameState()
{
	TimeLimit = 60.0f;
	
	SpawnedEnemyCount = 0;
	KilledEnemyCount = 0;

	SpawnAllEnemyCount = 10;
	SpawnPatrolEnemyCount = 3;
	IsWaveTriggered = false;
	CanActiveArrivalPoint = true;
}
