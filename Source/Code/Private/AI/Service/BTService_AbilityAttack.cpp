// Copyright 2022 Khanin Vladimir (FAZIC)

#include "AI/Service/BTService_AbilityAttack.h"
#include "AI/UnrealTestAIController.h"
#include "Character/UnrealTestCharacter.h"
#include <BehaviorTree/BehaviorTreeComponent.h>
#include <BehaviorTree/Blackboard/BlackboardKeyType_Object.h>
#include <AIController.h>
#include <BehaviorTree/BlackboardComponent.h>

#include "AIModule/Classes/Perception/AISense_Sight.h"

#include "AbilitySystem/UnrealTestAbilitySystemComponent.h"
#include "AbilitySystem/Ability/UTFireGameplayAbility.h"
#include "AbilitySystemGlobals.h"

#include "System/UnrealTestGameplayTags.h"

void UBTService_AbilityAttack::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	auto ControllerPtr = Cast<AUnrealTestAIController>(OwnerComp.GetAIOwner());
	if (!ControllerPtr)
		return;

	auto ActiveTargetPtr = ControllerPtr->GetActiveTarget();
	auto CharacterOwnerPtr = ControllerPtr->GetCharacterOwner();
	auto PerseptionPtr = ControllerPtr->GetPerceptionCmp();

	if (!CharacterOwnerPtr || !ActiveTargetPtr || !PerseptionPtr)
		return;

	auto ASC = Cast<UUnrealTestAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CharacterOwnerPtr));
	if (!ASC)
		return;

	const auto kAISightSenseID = UAISense::GetSenseID<UAISense_Sight>();
	if (!PerseptionPtr->HasActiveStimulus(*ActiveTargetPtr, kAISightSenseID))
		return;

	const auto TargetActorLocation = ActiveTargetPtr->GetActorLocation();
	const auto CharacterLocation = CharacterOwnerPtr->GetActorLocation();
	const auto DistanceSqr = FVector::DistSquared(TargetActorLocation, CharacterLocation);
	const auto ActorForwardVector = CharacterOwnerPtr->GetActorForwardVector();
	auto Dir2AimXY = FVector(TargetActorLocation.X - CharacterLocation.X, TargetActorLocation.Y - CharacterLocation.Y, 0.f);
	Dir2AimXY.Normalize();

	const auto CosAngleToAim = FVector::DotProduct(ActorForwardVector, Dir2AimXY);
	const FUnrealTestGameplayTags& GameplayTags = FUnrealTestGameplayTags::Get();

	FGameplayTagContainer AbilityTag;
	AbilityTag.AddTag(GameplayTags.Skill_First);

	ASC->TryActivateAbilitiesByTag(AbilityTag);
}
