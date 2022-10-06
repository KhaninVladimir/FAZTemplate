// Copyright 2022 Khanin Vladimir (FAZIC)


#include "Character/Component/UTHealthComponent.h"
#include "Character/UnrealTestCharacter.h"
#include "Net/UnrealNetwork.h"
#include "System/UnrealTestGameplayTags.h"
#include "UnrealTestLog.h"

#include "GameplayEffectExtension.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystem/UnrealTestAbilitySystemComponent.h"
#include "AbilitySystem/Attribute/UnrealTestAttributeSet.h"


UUTHealthComponent::UUTHealthComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bDead = false;
}

void UUTHealthComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UUTHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UUTHealthComponent, bDead);
}

void UUTHealthComponent::InitializeWithAbilitySystem(UUnrealTestAbilitySystemComponent* InASC)
{
	Super::InitializeWithAbilitySystem(InASC);

	auto ASC = GetAbilitySystemComponent();
	auto att = GetAttributeSet();
	if (!ASC || !att)
		return;

	// Register to listen for attribute changes.
	ASC->GetGameplayAttributeValueChangeDelegate(UUnrealTestAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
	ASC->GetGameplayAttributeValueChangeDelegate(UUnrealTestAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	ASC->SetNumericAttributeBase(UUnrealTestAttributeSet::GetHealthAttribute(), att->GetMaxHealth());

	att->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);

	ClearGameplayTags();

	OnHealthChanged.Broadcast(this, GetHealth(), GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, GetMaxHealth(), GetMaxHealth(), nullptr);
}

void UUTHealthComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();

	if (auto att = GetAttributeSet())
	{
		att->OnOutOfHealth.RemoveAll(this);
	}

	Super::UninitializeFromAbilitySystem();
}

void UUTHealthComponent::ClearGameplayTags()
{
	const FUnrealTestGameplayTags& GameplayTags = FUnrealTestGameplayTags::Get();
	if (auto ASC = GetAbilitySystemComponent())
		ASC->SetLooseGameplayTagCount(GameplayTags.State_Dead, 0);
}

float UUTHealthComponent::GetHealth() const
{
	return (GetAttributeSet() ? GetAttributeSet()->GetHealth() : 0.0f);
}

float UUTHealthComponent::GetMaxHealth() const
{
	return (GetAttributeSet() ? GetAttributeSet()->GetMaxHealth() : 0.0f);
}

float UUTHealthComponent::GetHealthNormalized() const
{
	if (auto att = GetAttributeSet())
	{
		const float Health = att->GetHealth();
		const float MaxHealth = att->GetMaxHealth();

		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}

	return 0.0f;
}

static AActor* GetInstigatorFromAttrChangeData(const FOnAttributeChangeData& ChangeData)
{
	if (const FGameplayEffectModCallbackData* data = ChangeData.GEModData)
	{
		const FGameplayEffectContextHandle& EffectContext = data->EffectSpec.GetEffectContext();
		return EffectContext.GetOriginalInstigator();
	}

	return nullptr;
}

void UUTHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	OnHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void UUTHealthComponent::HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	OnMaxHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, GetInstigatorFromAttrChangeData(ChangeData));
}

void UUTHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	bDead = true;
	auto ASC = GetAbilitySystemComponent();
	if (!ASC)
		return;

	// Send the "GameplayEvent.Death" gameplay event through the owner's ability system.  This can be used to trigger a death gameplay ability.
	const FUnrealTestGameplayTags& GameplayTags = FUnrealTestGameplayTags::Get();

	FGameplayEventData Payload;
	Payload.EventTag = GameplayTags.GameplayEvent_Death;
	Payload.Instigator = DamageInstigator;
	Payload.Target = ASC->GetAvatarActor();
	Payload.OptionalObject = DamageEffectSpec.Def;
	Payload.ContextHandle = DamageEffectSpec.GetEffectContext();
	Payload.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
	Payload.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
	Payload.EventMagnitude = DamageMagnitude;

	FScopedPredictionWindow NewScopedWindow(ASC, true);
	ASC->HandleGameplayEvent(Payload.EventTag, &Payload);

	OnDying.Broadcast(DamageMagnitude, DamageInstigator, DamageCauser);
	OnDeath();
}

void UUTHealthComponent::OnRep_DeathState(bool OldDeathState)
{
	if (bDead && GetCharacterOwner())
		GetCharacterOwner()->OnDeath();
}

void UUTHealthComponent::OnDeath()
{
	if (!bDead)
		return;

	const FUnrealTestGameplayTags& GameplayTags = FUnrealTestGameplayTags::Get();

	if (auto ASC = GetAbilitySystemComponent())
		ASC->AddLooseGameplayTag(GameplayTags.State_Dead);

	AActor* Owner = GetOwner();
	if (!Owner)
		return;

	Owner->ForceNetUpdate();
}

void UUTHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)
{
	if (!bDead && GetAbilitySystemComponent())
	{
		HandleOutOfHealth(GetOwner(), GetOwner(), FGameplayEffectSpec(), 1000.f);
	}
}
