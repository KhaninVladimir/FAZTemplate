// Copyright 2022 Khanin Vladimir (FAZIC)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "Runtime/AIModule/Classes/GenericTeamAgentInterface.h"
#include "UnrealTestPlayerTeam.generated.h"

class AUnrealTestPlayerState;

/**
 * class for teams
 */
UCLASS()
class UNREALTEST_API AUnrealTestPlayerTeam : public AInfo, public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	AUnrealTestPlayerTeam();

	// Interface IGenericTeamAgentInterface
	FGenericTeamId GetTeamID() const;

	void SetTeamInfo(FGenericTeamId TeamID, FString TeamName);

	void SetTeamName(const FString& NewTeamName);

	UFUNCTION(BlueprintCallable)
	const FString& GetTeamName() const;

	void AddMember(AUnrealTestPlayerState* PlayerState);

	void RemoveMember(AUnrealTestPlayerState* PlayerState);

	UFUNCTION(BlueprintPure)
	const TArray<AUnrealTestPlayerState*>& GetTeamMembers() const;

	bool IsTeamMember(AUnrealTestPlayerState* PlayerState) const;
protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type Reason) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnTeamMemberAdded(AUnrealTestPlayerState* AddedMemberState);
	virtual void OnTeamMemberRemoved(AUnrealTestPlayerState* RemovedMemberState);

private:

	struct FTeamMemberData
	{
		FDelegateHandle OnMemberScoreChangedDelegate;
	};

	UPROPERTY(Transient, Replicated)
	FGenericTeamId TeamID;

	UPROPERTY(Transient, Replicated)
	FString TeamName;

	UPROPERTY(Transient, ReplicatedUsing = OnRep_TeamMembers)
	TArray<AUnrealTestPlayerState*> TeamMembers;

	TMap<AUnrealTestPlayerState*, FTeamMemberData> TeamMembmersData;

	UFUNCTION()
	void OnRep_TeamMembers();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_RemoveMember(AUnrealTestPlayerState* MemberState);
};
