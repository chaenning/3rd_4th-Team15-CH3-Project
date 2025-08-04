#include "AI/Data/Base/XVBlackBoardDataBase.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Bool.h"

UXVBlackBoardDataBase::UXVBlackBoardDataBase()
{
    FBlackboardEntry Entry;
    TObjectPtr<UBlackboardKeyType> KeyType;

    // Perception 관련 vector 키 등록
    Entry.EntryName = TEXT("MyLocation");
    KeyType = UBlackboardKeyType_Vector::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
    
    Entry.EntryName = TEXT("TargetLocation");
    KeyType = UBlackboardKeyType_Vector::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
    
    // Perception 관련 Bool 키 등록
    Entry.EntryName = TEXT("CanSeeTarget");
    KeyType = UBlackboardKeyType_Bool::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
    
    Entry.EntryName = TEXT("IsInvestigating");
    KeyType = UBlackboardKeyType_Bool::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
    
    Entry.EntryName = TEXT("AIIsAttacking");
    KeyType = UBlackboardKeyType_Bool::StaticClass()->GetDefaultObject<UBlackboardKeyType>();
    Entry.KeyType = KeyType;
    Keys.Add(Entry);
}
