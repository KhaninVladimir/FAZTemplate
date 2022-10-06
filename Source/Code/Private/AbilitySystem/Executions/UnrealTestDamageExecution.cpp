// Copyright 2022 Khanin Vladimir (FAZIC)


#include "AbilitySystem/Executions/UnrealTestDamageExecution.h"
#include "AbilitySystem/UnrealTestAbilitySystemComponent.h"
#include "AbilitySystem/Attribute/UnrealTestAttributeSet.h"
#include "System/UnrealTestGameplayTags.h"
#include "GameFramework/Pawn.h"
#include "Player/UnrealTestPlayerState.h"

UUnrealTestDamageExecution::UUnrealTestDamageExecution()
{
}

void UUnrealTestDamageExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	UAbilitySystemComponent* SourceAbilitySystemComponent = ExecutionParams.GetSourceAbilitySystemComponent();
	
	AActor* HitActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;
	AActor* SourceActor = SourceAbilitySystemComponent ? SourceAbilitySystemComponent->GetAvatarActor() : nullptr;

	if (HitActor != SourceActor)
	{
		APawn* Pawn_A = Cast<APawn>(HitActor);
		APawn* Pawn_B = Cast<APawn>(SourceActor);
		AUnrealTestPlayerState* PS_A = Pawn_A ? Cast<AUnrealTestPlayerState>(Pawn_A->GetPlayerState()) : nullptr;
		AUnrealTestPlayerState* PS_B = Pawn_B ? Cast<AUnrealTestPlayerState>(Pawn_B->GetPlayerState()) : nullptr;
		
		FGenericTeamId Team_A = PS_A ? PS_A->GetTeamID() : 255;
		FGenericTeamId Team_B = PS_B ? PS_B->GetTeamID() : 255;

		if (Team_A == Team_B)
			return;
	}

	const FUnrealTestGameplayTags& GameplayTags = FUnrealTestGameplayTags::Get();
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	float Damage = Spec.GetSetByCallerMagnitude(GameplayTags.Gameplay_Damage, false, 0.f);
	Damage = FMath::Max<float>(Damage, 0.f);

	if (Damage > 0.f)
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UUnrealTestAttributeSet::GetHealthAttribute(), EGameplayModOp::Additive, -Damage));

#endif // #if WITH_SERVER_CODE
}
