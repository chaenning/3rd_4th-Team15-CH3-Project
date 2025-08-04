#pragma once

#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(Location) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 100.f, 24, FColor::Red, false, 60.f, 0, 1.f); // 원형 디버깅 툴 : 지정된 위치에 구체 생성
#define DRAW_LINE(Start, End) if(GetWorld()) DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 60.f, 0, 1.f);			 // 라인 디버깅 툴 : 두 점사이에 선을 그림
#define DRAW_POINT(Location) if(GetWorld()) DrawDebugPoint(GetWorld(), Location, 25.f, FColor::Yellow, false, 60.f, 0);			 // 포인 디버깅 툴  : 지정된 위치에 점 생성

#define LENGTH_VECTOR(ActorLocation, ForwardLocation) (ActorLocation + (ForwardLocation * 100.f))								 // 길이 계산 : 방향 백터 계산

DECLARE_LOG_CATEGORY_EXTERN(Log_XV_AI, Log, All);
