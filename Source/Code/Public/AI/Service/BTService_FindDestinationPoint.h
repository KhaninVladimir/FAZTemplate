// Copyright 2022 Khanin Vladimir (FAZIC)

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"
#include "BTService_FindDestinationPoint.generated.h"

class AUnrealTestAIController;
/**
 * 
 */
UCLASS()
class UNREALTEST_API UBTService_FindDestinationPoint : public UBTService_BlackboardBase
{
	GENERATED_BODY()

protected:
	/** update next tick interval
	* this function should be considered as const (don't modify state of object) if node is not instanced! */
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	bool CalculateDestinationPoint(FVector& OutDestination, const AUnrealTestAIController& NpcCntrlRef) const;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	float MinRangeTargetDestinationPoint = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	float MaxRangeTargetDestinationPoint = 2500.f;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	float MinRangeTargetDestinationPointRandom = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	float MaxRangeTargetDestinationPointRandom = 2500.f;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	float MinRangeNoTargetDestinationPoint = 20000.f;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	float MaxRangeNoTargetDestinationPoint = 40000.f;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	float MinRangeGetCloser = 400.f;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	float MaxRangeGetCloser = 800.f;

	UPROPERTY(EditDefaultsOnly, Category = AI)
	float MaxFarDistance = 20000.f;

};
