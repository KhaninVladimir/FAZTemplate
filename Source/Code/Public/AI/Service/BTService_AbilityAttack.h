// Copyright 2022 Khanin Vladimir (FAZIC)

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_AbilityAttack.generated.h"

/**
 * 
 */
UCLASS()
class UNREALTEST_API UBTService_AbilityAttack : public UBTService_BlackboardBase
{
	GENERATED_BODY()
	
protected:
	/** update next tick interval
	* this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
