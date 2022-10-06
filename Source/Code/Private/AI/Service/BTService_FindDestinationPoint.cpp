// Copyright 2022 Khanin Vladimir (FAZIC)

#include "AI/Service/BTService_FindDestinationPoint.h"
#include "AI/UnrealTestAIController.h"
#include "AI/NavigationSystemBase.h"
#include "NavigationSystem.h"
#include "Character/UnrealTestCharacter.h"
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Object.h>
#include <BehaviorTree/BlackboardComponent.h>
#include <EngineUtils.h>

void UBTService_FindDestinationPoint::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const auto ControllerPtr = Cast<AUnrealTestAIController>(OwnerComp.GetAIOwner());
	if (!ControllerPtr)
		return;

	FVector OutDestination;
	if (CalculateDestinationPoint(OutDestination, *ControllerPtr))
	{
		ControllerPtr->SetTargetLocation(OutDestination);
	}
	else
	{
		ControllerPtr->ClearTargetLocation();
	}
}

bool UBTService_FindDestinationPoint::CalculateDestinationPoint(FVector& OutDestination, const AUnrealTestAIController& NpcCntrlRef) const
{
	const auto NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys)
		return false;

	const auto OwnerCharacterPtr = NpcCntrlRef.GetCharacterOwner();
	if (!OwnerCharacterPtr)
		return false;

	if (!OwnerCharacterPtr->IsAlive() || !OwnerCharacterPtr->IsPawnControlled())
		return false;

	const auto SelfLocation = OwnerCharacterPtr->GetActorLocation();
	FNavLocation NavLocation;

	if (const auto ActiveTargetPtr = NpcCntrlRef.GetActiveTarget())
	{
		const auto ActorLocation = ActiveTargetPtr->GetActorLocation();
		FVector NewLocation;

		const auto MinRangeGetCloserSqr = FMath::Square(MinRangeGetCloser);
		const auto MaxRangeGetCloserSqr = FMath::Square(MaxRangeGetCloser);
		const auto MaxFarDistanceSqr = FMath::Square(MaxRangeGetCloser);
		auto DistanceSqr = FVector::DistSquared(ActorLocation, SelfLocation);
		if (DistanceSqr < FMath::RandRange(MinRangeGetCloserSqr, MaxRangeGetCloserSqr))
		{
			const auto dir = (SelfLocation - ActorLocation).Normalize();
			NewLocation = SelfLocation + dir * FMath::RandRange(MinRangeTargetDestinationPoint, MaxRangeTargetDestinationPoint);
		}
		else if ( DistanceSqr < MaxFarDistanceSqr)
		{			
			//Create more smart algorithm for calculation radius and location
			NewLocation = (SelfLocation + ActorLocation) * 0.5f;
		}
		else
		{
			// For very far distance move direct to target
			const auto dir = (ActorLocation - SelfLocation).Normalize();
			NewLocation = SelfLocation + dir * MaxFarDistanceSqr * 0.5f;
		}

		const auto Radius = FMath::RandRange(MinRangeTargetDestinationPoint, MaxRangeTargetDestinationPoint);
		if (NavSys->GetRandomPointInNavigableRadius(NewLocation, Radius, NavLocation))
		{
			OutDestination = NavLocation.Location;
			return true;
		}
	}

	auto Radius = FMath::RandRange(MinRangeNoTargetDestinationPoint, MaxRangeNoTargetDestinationPoint);
	if (NavSys->GetRandomPointInNavigableRadius(SelfLocation, Radius, NavLocation))
	{
		OutDestination = NavLocation.Location;
		return true;
	}

	return false;
}