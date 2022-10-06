// Copyright 2022 Khanin Vladimir (FAZIC)


#include "AbilitySystem/UnrealTestGameplayAbility.h"
#include "AbilitySystem/UnrealTestAbilitySystemComponent.h"
#include "Character/UnrealTestCharacter.h"
#include "Player/UnrealTestPlayerController.h"
#include "System/UnrealTestGameplayTags.h"
#include "UnrealTestLog.h"


UUnrealTestGameplayAbility::UUnrealTestGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

UUnrealTestAbilitySystemComponent* UUnrealTestGameplayAbility::GetUTAbilitySystemComponentFromActorInfo() const
{
	if (!ASCPtr.IsValid() && CurrentActorInfo)
		ASCPtr = Cast<UUnrealTestAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get());

	return ASCPtr.Get();
}

AUnrealTestPlayerController* UUnrealTestGameplayAbility::GetUTPlayerControllerFromActorInfo() const
{
	return Cast<AUnrealTestPlayerController>(GetControllerFromActorInfo());
}

AController* UUnrealTestGameplayAbility::GetControllerFromActorInfo() const
{
	if (ControllerPtr.IsValid())
		return ControllerPtr.Get();

	if (!CurrentActorInfo)
		return nullptr;

	if (AController* PC = CurrentActorInfo->PlayerController.Get())
	{
		ControllerPtr = PC;
		return ControllerPtr.Get();
	}

	// Look for a player controller or pawn in the owner chain.
	AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
	while (TestActor)
	{
		if (AController* C = Cast<AController>(TestActor))
		{
			ControllerPtr = C;
			return ControllerPtr.Get();
		}

		if (APawn* Pawn = Cast<APawn>(TestActor))
		{
			ControllerPtr = Pawn->GetController();
			return ControllerPtr.Get();
		}

		TestActor = TestActor->GetOwner();
	}

	return nullptr;
}

AUnrealTestCharacter* UUnrealTestGameplayAbility::GetUTCharacterFromActorInfo() const
{
	if (!CharacterPtr.IsValid() && CurrentActorInfo)
		CharacterPtr = Cast<AUnrealTestCharacter>(CurrentActorInfo->AvatarActor.Get());

	return CharacterPtr.Get();
}