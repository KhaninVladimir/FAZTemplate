// Copyright 2022 Khanin Vladimir (FAZIC)


#include "AbilitySystem/Ability/UTFireGameplayAbility.h"
#include "AbilitySystem/Actors/UTFireActor.h"
#include "AbilitySystemComponent.h"
#include "Character/UnrealTestCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "System/UnrealTestGameplayTags.h"


UUTFireGameplayAbility::UUTFireGameplayAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	Range = 1000.0f;
	Damage = 50.0f;
}

void UUTFireGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	AUnrealTestCharacter* Char = Cast<AUnrealTestCharacter>(GetAvatarActorFromActorInfo());
	if (!Char)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	if (!Char->HasAuthority())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	// Only spawn projectiles on the Server.
	FTransform MuzzleTransform = Char->GetMesh()->GetSocketTransform(FName("ShootSocket"));

	FVector Start = MuzzleTransform.GetLocation();
	FVector End = Start + Char->GetActorForwardVector() * Range;
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, End);

	FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, GetAbilityLevel());

	const FUnrealTestGameplayTags& GameplayTags = FUnrealTestGameplayTags::Get();

	// Pass the damage to the Damage Execution Calculation through a SetByCaller value on the GameplayEffectSpec
	DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(GameplayTags.Gameplay_Damage, Damage);

	MuzzleTransform.SetRotation(Rotation.Quaternion());
	MuzzleTransform.SetScale3D(FVector(1.0f));

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AUTFireActor* Projectile = GetWorld()->SpawnActorDeferred<AUTFireActor>(ProjectileClass, MuzzleTransform, GetOwningActorFromActorInfo(),
		Char, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Projectile->DamageEffectSpecHandle = DamageEffectSpecHandle;
	Projectile->Range = Range;
	Projectile->FinishSpawning(MuzzleTransform);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
