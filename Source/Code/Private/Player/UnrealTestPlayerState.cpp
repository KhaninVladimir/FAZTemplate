// Copyright 2022 Khanin Vladimir (FAZIC)


#include "Player/UnrealTestPlayerState.h"
#include "Player/UnrealTestPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Game/UnrealTestGameState.h"
#include "Game/UnrealTestGameMode.h"
#include "Game/UnrealTestPlayerTeam.h"

#include "Character/UnrealTestCharacter.h"
#include "Character/Component/UTHealthComponent.h"
#include "AbilitySystem/UnrealTestAbilitySystemComponent.h"
#include "AbilitySystem/Attribute/UnrealTestAttributeSet.h"


AUnrealTestPlayerState::AUnrealTestPlayerState()
{
	PrimaryActorTick.bCanEverTick = true;
	PlayerPhase = EPlayerPhase::PP_Preparation;

	SetActorTickInterval(1.f);

	AbilitySystemComponent = CreateDefaultSubobject<UUnrealTestAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	// Mixed mode means we only are replicated the GEs to ourself, not the GEs to simulated proxies. If another GDPlayerState (Hero) receives a GE,
	// we won't be told about it by the Server. Attributes, GameplayTags, and GameplayCues will still replicate to us.
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Set PlayerState's NetUpdateFrequency to the same as the Character.
	// Default is very low for PlayerStates and introduces perceived lag in the ability system.
	NetUpdateFrequency = 10.0f;
}

void AUnrealTestPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS(AUnrealTestPlayerState, TimeOfDeath, Params);
	DOREPLIFETIME_WITH_PARAMS(AUnrealTestPlayerState, PlayerTeam, Params);
	DOREPLIFETIME_WITH_PARAMS(AUnrealTestPlayerState, PlayerPhase, Params);

	DOREPLIFETIME_WITH_PARAMS(AUnrealTestPlayerState, KillCount, Params);
	DOREPLIFETIME_WITH_PARAMS(AUnrealTestPlayerState, DeathCount, Params);
	DOREPLIFETIME_WITH_PARAMS(AUnrealTestPlayerState, AssistCount, Params);

	Params.Condition = ELifetimeCondition::COND_OwnerOnly;
	DOREPLIFETIME_WITH_PARAMS(AUnrealTestPlayerState, SelectedPlayerPawn, Params);
	DOREPLIFETIME_WITH_PARAMS(AUnrealTestPlayerState, SelectedCharacterIndex, Params);
	
}

void AUnrealTestPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (auto GS = GetUTGameState())
	{
		CharactersList = GS->GetCharactersList();
	}

	if (HasAuthority())
	{
		if (auto ASC = GetAbilitySystemComponent())
			ASC->InitStats(UUnrealTestAttributeSet::StaticClass(), nullptr);
	}
}

void AUnrealTestPlayerState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (HasAuthority() && CanRespawn())
	{
		AUnrealTestGameMode* GM = GetUTGameMode();
		AController* Controller = GetController();
		if (Controller && GM)
		{
			GM->RestartPlayer(Controller);
			SetTimeOfDeath(0.f);
		}
	}
}

void AUnrealTestPlayerState::SetPlayerPhase(EPlayerPhase NewPhase)
{
	// If this is a client, then the State has already been replicated and changed.
	if (HasAuthority())
	{
		PlayerPhase = NewPhase;
		switch (PlayerPhase)
		{
		case EPlayerPhase::PP_Dead:
				SetPlayerPhase(EPlayerPhase::PP_DeadAndWaitRespawn);
			break;
		case EPlayerPhase::PP_Preparation:
		case EPlayerPhase::PP_Battle:
		case EPlayerPhase::PP_DeadAndWaitRespawn:
			if (auto GS = GetUTGameState())
			{
				SetTimeOfDeath(GS->GetReplicatedWorldTimeSeconds());
			}
		default:
			break;
		}
	}

	OnPlayerPhaseChanged.Broadcast(PlayerPhase);
}


bool AUnrealTestPlayerState::CanRespawn() const
{
	return	GetRespawnTimer() < 0.f && SelectedPlayerPawn && !GetPawn() &&
			GetPlayerPhase() == EPlayerPhase::PP_DeadAndWaitRespawn;
}

void AUnrealTestPlayerState::SetTimeOfDeath(float Time)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(AUnrealTestPlayerState, TimeOfDeath, this);
	TimeOfDeath = Time;
}

float AUnrealTestPlayerState::GetRespawnTimer() const
{
	if (TimeOfDeath > 0)
	{
		auto PC = GetController();
		auto GS = GetUTGameState();
		if (PC && GS)
		{
			return FMath::CeilToFloat(GS->GetPlayerRespawnDelay(PC) - (GS->GetReplicatedWorldTimeSeconds() - TimeOfDeath));
		}
	}
	return -1.f;
}

void AUnrealTestPlayerState::AddKillCount(int32 Count)
{
	if (HasAuthority() && IsMatchActive())
	{
		SetKillCount(GetKillCount() + Count);
	}
}

void AUnrealTestPlayerState::SetKillCount(int32 Count)
{
#if UE_SERVER || WITH_EDITOR
	if (HasAuthority() && IsMatchActive())
	{
		KillCount = Count;
		MARK_PROPERTY_DIRTY_FROM_NAME(AUnrealTestPlayerState, KillCount, this);
	}
#endif
}

int32 AUnrealTestPlayerState::GetKillCount() const
{
	return KillCount;
}

void AUnrealTestPlayerState::AddDeathCount(int32 Count)
{
#if UE_SERVER || WITH_EDITOR
	if (HasAuthority() && IsMatchActive())
	{
		SetDeathCount(GetDeathCount() + Count);
		SetPlayerPhase(EPlayerPhase::PP_Dead);
	}
#endif
}

void AUnrealTestPlayerState::SetDeathCount(int32 Count)
{
#if UE_SERVER || WITH_EDITOR
	if (HasAuthority() && IsMatchActive())
	{
		DeathCount = Count;
		MARK_PROPERTY_DIRTY_FROM_NAME(AUnrealTestPlayerState, DeathCount, this);
	}
#endif
}

int32 AUnrealTestPlayerState::GetDeathCount() const
{
	return DeathCount;
}

void AUnrealTestPlayerState::AddAssistCount(int32 Count)
{
	if (HasAuthority() && IsMatchActive())
	{
		SetAssistCount(GetAssistCount() + Count);
	}
}

void AUnrealTestPlayerState::SetAssistCount(int32 Count)
{
#if UE_SERVER || WITH_EDITOR
	if (HasAuthority() && IsMatchActive())
	{
		AssistCount = Count;
		MARK_PROPERTY_DIRTY_FROM_NAME(AUnrealTestPlayerState, AssistCount, this);
	}
#endif
}

int32 AUnrealTestPlayerState::GetAssistCount() const
{
	return AssistCount;
}

FPlayerStatistic AUnrealTestPlayerState::GetPlayerStatistic()
{
	FPlayerStatistic PlayerStatistic;
	PlayerStatistic.PlayerName = GetPlayerName();
	PlayerStatistic.KillsCount = GetKillCount();
	PlayerStatistic.DeathCount = GetDeathCount();
	PlayerStatistic.AssistCount = GetAssistCount();
	PlayerStatistic.ScoreCount = GetScore();

	return PlayerStatistic;
}

void AUnrealTestPlayerState::OnRep_Score()
{
	Super::OnRep_Score();
}

void AUnrealTestPlayerState::OnRep_PlayerPhase()
{
	SetPlayerPhase(PlayerPhase);
}

void AUnrealTestPlayerState::OnRep_PlayerStatistic()
{
	if (auto PC = Cast<AUnrealTestPlayerController>(GetOwner()))
	{
		PC->Client_OnPlayerStatisticChanges.Broadcast(GetPlayerStatistic(), this);
	}
}

AUnrealTestPlayerTeam* AUnrealTestPlayerState::GetPlayerTeam() const
{
	return PlayerTeam;
}

void AUnrealTestPlayerState::SetPlayerTeam(AUnrealTestPlayerTeam* Team)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(AUnrealTestPlayerState, PlayerTeam, this);
	PlayerTeam = Team;
}

void AUnrealTestPlayerState::SetPlayerPawn(TSubclassOf<ACharacter> NewClass)
{
	MARK_PROPERTY_DIRTY_FROM_NAME(AUnrealTestPlayerState, SelectedPlayerPawn, this);
	SelectedPlayerPawn = NewClass;

	if (!IsMatchActive())
		return;

	SetPlayerPhase(EPlayerPhase::PP_DeadAndWaitRespawn);

	if (!GetPawn())
		return;

	auto Health = UUTHealthComponent::FindHealthComponent(GetPawn());

	if (Health)
	{
		Health->DamageSelfDestruct();
		return;
	}

	GetPawn()->DetachFromControllerPendingDestroy();
}

void AUnrealTestPlayerState::SetPlayerPawn(int32 CharacterIndex)
{
	if (!IsValid(CharactersList))
	{
		if (auto GS = GetUTGameState())
			CharactersList = GS->GetCharactersList();
	}

	if (!IsValid(CharactersList))
		return;

	TArray<FUTCharacterInfo> List = CharactersList->CharacterList;
	if (SelectedCharacterIndex == CharacterIndex || !List.IsValidIndex(CharacterIndex))
		return;

	MARK_PROPERTY_DIRTY_FROM_NAME(AUnrealTestPlayerState, SelectedCharacterIndex, this);
	SelectedCharacterIndex = CharacterIndex;
	OnSelectCharacter.Broadcast(SelectedCharacterIndex);

	SetPlayerPawn(List[SelectedCharacterIndex].CharacterClass);
}

void AUnrealTestPlayerState::OnRep_SelectedCharacterIndex()
{
	OnSelectCharacter.Broadcast(SelectedCharacterIndex);
}

bool AUnrealTestPlayerState::IsMatchActive() const
{
	if (!GetWorld()) return false;

	if (AUnrealTestGameMode* GM = GetUTGameMode())
	{
		return GM->IsMatchInProgress();
	}

	return false;
}

FGenericTeamId AUnrealTestPlayerState::GetTeamID() const
{
	if (!PlayerTeam)
	{
		return FGenericTeamId::NoTeam;
	}

	return PlayerTeam->GetTeamID();
}

AUnrealTestGameState* AUnrealTestPlayerState::GetUTGameState() const
{
	if (!GameStatePtr.IsValid() && GetWorld())
		GameStatePtr = GetWorld()->GetGameState<AUnrealTestGameState>();

	return GameStatePtr.Get();
}

AUnrealTestGameMode* AUnrealTestPlayerState::GetUTGameMode() const
{
	if (!GameModePtr.IsValid() && GetWorld())
		GameModePtr = GetWorld()->GetAuthGameMode<AUnrealTestGameMode>();

	return GameModePtr.Get();
}

AController* AUnrealTestPlayerState::GetController() const
{
	if (!ControllerPtr.IsValid() && GetOwner())
		ControllerPtr = Cast<AController>(GetOwner());

	return ControllerPtr.Get();
}

UAbilitySystemComponent* AUnrealTestPlayerState::GetAbilitySystemComponent() const
{
	return GetUTAbilitySystemComponent();
}

const UUnrealTestAttributeSet* AUnrealTestPlayerState::GetAttributeSet() const
{
	if (!IsValid(AttributeSet))
		AttributeSet = GetAbilitySystemComponent()->GetSet<UUnrealTestAttributeSet>();

	return AttributeSet;
}