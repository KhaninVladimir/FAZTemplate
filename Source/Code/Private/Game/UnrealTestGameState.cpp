// Copyright 2022 Khanin Vladimir (FAZIC)


#include "Game/UnrealTestGameState.h"
#include "Game/UnrealTestGameMode.h"
#include "Game/UnrealTestPlayerTeam.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include <EngineUtils.h>
#include "Kismet/GameplayStatics.h"
#include "Player/UnrealTestPlayerState.h"

AUnrealTestGameState::AUnrealTestGameState()
{

}

void AUnrealTestGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
	BP_AddPlayerState(PlayerState);
}

void AUnrealTestGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
	BP_RemovePlayerState(PlayerState);
}

void AUnrealTestGameState::BP_AddPlayerState_Implementation(APlayerState* PlayerState)
{

}

void AUnrealTestGameState::BP_RemovePlayerState_Implementation(APlayerState* PlayerState)
{

}

TArray<APlayerState*> AUnrealTestGameState::SortPlayerArray()
{
	auto SortedPlayerArray = PlayerArray;
	SortedPlayerArray.Sort([&](const APlayerState& A, const APlayerState& B)
	{
		auto PS_A = Cast<AUnrealTestPlayerState>(&A);
		auto PS_B = Cast<AUnrealTestPlayerState>(&B);
		if (PS_A && PS_B)
			return PS_A->GetKillCount() < PS_B->GetKillCount();
		return false;
	});
	return SortedPlayerArray;
}

void AUnrealTestGameState::AddPlayerToTeam(AUnrealTestPlayerState* Player, AUnrealTestPlayerTeam* Team)
{
	if (!Player || !Team)
		return;

	Team->AddMember(Player);
	Player->SetPlayerTeam(Team);
}

void AUnrealTestGameState::AddPlayerToTeam(AUnrealTestPlayerState* Player, FGenericTeamId TeamId)
{
	AddPlayerToTeam(Player, GetTeam(TeamId));
}

void AUnrealTestGameState::AddTeam(AUnrealTestPlayerTeam* Team)
{
	if (!IsValid(Team))
	{
		return;
	}

	if (TeamsList.AddUnique(Team) != INDEX_NONE)
	{
		EventTeamAdded.Broadcast(Team);
	}
}

void AUnrealTestGameState::RemoveTeam(AUnrealTestPlayerTeam* Team)
{
	if (!IsValid(Team))
	{
		return;
	}

	if (TeamsList.Remove(Team) > 0)
	{
		EventTeamRemoved.Broadcast(Team);
	}
}

AUnrealTestPlayerTeam* AUnrealTestGameState::GetTeam(FGenericTeamId TeamID) const
{
	auto FindByTeamID = [TeamID](const AUnrealTestPlayerTeam* Team) {
		return Team->GetTeamID() == TeamID;
	};
	AUnrealTestPlayerTeam* const* Team = TeamsList.FindByPredicate(FindByTeamID);

	return Team == nullptr ? nullptr : *Team;
}

AUnrealTestGameMode* AUnrealTestGameState::GetGameMode() const
{
	if (!GameModePtr.IsValid() && GetWorld())
	{
		GameModePtr = GetWorld()->GetAuthGameMode<AUnrealTestGameMode>();
	}
	return GameModePtr.Get();
}