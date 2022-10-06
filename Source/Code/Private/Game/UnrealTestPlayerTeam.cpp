// Copyright 2022 Khanin Vladimir (FAZIC)


#include "Game/UnrealTestPlayerTeam.h"
#include "Player/UnrealTestPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Game/UnrealTestGameState.h"


AUnrealTestPlayerTeam::AUnrealTestPlayerTeam()
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AUnrealTestPlayerTeam::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS(AUnrealTestPlayerTeam, TeamID, Params);
	DOREPLIFETIME_WITH_PARAMS(AUnrealTestPlayerTeam, TeamName, Params);
	
	DOREPLIFETIME(AUnrealTestPlayerTeam, TeamMembers);
}

void AUnrealTestPlayerTeam::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (AUnrealTestGameState* GS = World->GetGameState<AUnrealTestGameState>())
	{
		GS->AddTeam(this);
	}
}

void AUnrealTestPlayerTeam::EndPlay(EEndPlayReason::Type Reason)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (AUnrealTestGameState* GS = World->GetGameState<AUnrealTestGameState>())
	{
		GS->RemoveTeam(this);
	}

	Super::EndPlay(Reason);
}

void AUnrealTestPlayerTeam::SetTeamInfo(FGenericTeamId NewTeamID, FString NewTeamName)
{
	TeamID = NewTeamID;
	TeamName = NewTeamName;
	MARK_PROPERTY_DIRTY_FROM_NAME(AUnrealTestPlayerTeam, TeamID, this);
	MARK_PROPERTY_DIRTY_FROM_NAME(AUnrealTestPlayerTeam, TeamName, this);
}

FGenericTeamId AUnrealTestPlayerTeam::GetTeamID() const
{
	return TeamID;
}

void AUnrealTestPlayerTeam::SetTeamName(const FString& NewTeamName)
{
	TeamName = NewTeamName;
}

const FString& AUnrealTestPlayerTeam::GetTeamName() const
{
	return TeamName;
}

void AUnrealTestPlayerTeam::AddMember(AUnrealTestPlayerState* PlayerState)
{
	TeamMembers.AddUnique(PlayerState);
	OnTeamMemberAdded(PlayerState);
}

void AUnrealTestPlayerTeam::RemoveMember(AUnrealTestPlayerState* PlayerState)
{
	Multicast_RemoveMember(PlayerState);
	TeamMembers.Remove(PlayerState);
}

void AUnrealTestPlayerTeam::Multicast_RemoveMember_Implementation(AUnrealTestPlayerState* MemberState)
{
}

void AUnrealTestPlayerTeam::OnTeamMemberAdded(AUnrealTestPlayerState* AddedMemberState)
{
	check(AddedMemberState != nullptr);

	FTeamMemberData& MemberData = TeamMembmersData.Add(AddedMemberState);
}

void AUnrealTestPlayerTeam::OnTeamMemberRemoved(AUnrealTestPlayerState* RemovedMemberState)
{
	check(RemovedMemberState != nullptr);

	FTeamMemberData* MemberData = TeamMembmersData.Find(RemovedMemberState);
	if (MemberData != nullptr)
	{
		TeamMembmersData.Remove(RemovedMemberState);
	}
}

const TArray<AUnrealTestPlayerState*>& AUnrealTestPlayerTeam::GetTeamMembers() const
{
	return TeamMembers;
}

bool AUnrealTestPlayerTeam::IsTeamMember(AUnrealTestPlayerState* PlayerState) const
{
	return TeamMembers.Contains(PlayerState);
}

void AUnrealTestPlayerTeam::OnRep_TeamMembers()
{
	for (AUnrealTestPlayerState* MemberState : TeamMembers)
	{
		if (IsValid(MemberState))
		{
			FTeamMemberData* MemberData = TeamMembmersData.Find(MemberState);
			if (MemberData == nullptr)
			{
				OnTeamMemberAdded(MemberState);
			}
		}
	}
}

