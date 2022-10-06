// Copyright 2022 Khanin Vladimir (FAZIC)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Common/UnrealTestType.h"
#include "GenericTeamAgentInterface.h"
#include "Player/UnrealTestCharacterListData.h"
#include "UnrealTestGameState.generated.h"

class AUnrealTestGameMode;
class AUnrealTestPlayerTeam;
class AUnrealTestPlayerState;

/**
 * 
 */
UCLASS(Blueprintable, Meta = (ChildCanTick))
class AUnrealTestGameState : public AGameState
{
	GENERATED_BODY()
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FEventTeamChanges, AUnrealTestPlayerTeam*)
	FEventTeamChanges EventTeamAdded;
	FEventTeamChanges EventTeamRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnMatchStateChanged OnMatchStateChanged;

	float GetReplicatedWorldTimeSeconds() const { return ReplicatedWorldTimeSeconds; }

	void AddPlayerToTeam(AUnrealTestPlayerState* Player, AUnrealTestPlayerTeam* Team);
	void AddPlayerToTeam(AUnrealTestPlayerState* Player, FGenericTeamId TeamId);
	void AddTeam(AUnrealTestPlayerTeam* Team);
	void RemoveTeam(AUnrealTestPlayerTeam* Team);
	AUnrealTestPlayerTeam* GetTeam(FGenericTeamId TeamID) const;

	UUnrealTestCharacterListData* GetCharactersList() const { return CharactersList; }

protected:
	//It`s need on server and clients(UI)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameState")
	UUnrealTestCharacterListData* CharactersList;

	AUnrealTestGameState();
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;

	UFUNCTION(BlueprintNativeEvent)
	void BP_AddPlayerState(APlayerState* PlayerState);

	UFUNCTION(BlueprintNativeEvent)
	void BP_RemovePlayerState(APlayerState* PlayerState);

	UFUNCTION(BlueprintCallable)
	TArray<APlayerState*> SortPlayerArray();

	AUnrealTestGameMode* GetGameMode() const;
private:
	UPROPERTY(Transient)
	TArray<AUnrealTestPlayerTeam*> TeamsList;

	mutable TSoftObjectPtr<AUnrealTestGameMode> GameModePtr;
};
