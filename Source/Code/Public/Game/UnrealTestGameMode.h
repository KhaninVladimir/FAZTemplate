// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "UnrealTestGameMode.generated.h"

class AUnrealTestPlayerTeam;
class AUnrealTestGameState;
class AUnrealTestPlayerStart;
class AUnrealTestPlayerController;
class AUnrealTestAIController;
class AUnrealTestCharacter;

/*
* Base class for all game modes
*/
UCLASS(minimalapi)
class AUnrealTestGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AUnrealTestGameMode();
	virtual void RestartPlayer(AController* NewPlayer) override;
	int32 ConnectedPlayerNum() const { return ConnectedPlayers.Num(); }

	virtual void Killed(AController* Killer, AController* VictimPlayer, APawn* VictimPawn, AActor* DamageCauser);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode")
	TSubclassOf<AUnrealTestPlayerStart> GameModePlayerStarts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode|Bot")
	int32 MaxBotCount = 3;
	
	UPROPERTY(EditDefaultsOnly, Category = "GameMode|Bot")
	TArray<TSubclassOf<AUnrealTestCharacter>> BotSpawnedClasses;

	UPROPERTY(BlueprintReadOnly)
	TArray<AUnrealTestPlayerStart*> PlayerStarts;

	UPROPERTY(BlueprintReadOnly, Category = "GameMode")
	TArray<AUnrealTestAIController*> AIControllers;

	UPROPERTY(BlueprintReadOnly, Category = GameMode)
	TArray<AUnrealTestPlayerController*> ConnectedPlayers;

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void Logout(AController* Exiting) override;

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Tick(float DeltaSeconds);

	virtual void AssignPlayerToTeam(const AController* Player, AUnrealTestPlayerTeam* Team);
	AUnrealTestPlayerTeam* SetDefaultPlayerTeam(const AController* Player);
	void SpawnDefaultsTeams(int32 Count);
	AUnrealTestPlayerTeam* SpawnTeam(int32 TeamId);

	virtual class UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;
	virtual bool PlayerCanRestart_Implementation(APlayerController* Player) override;

	virtual void DestroyAIController(AController* AIController);
	virtual AUnrealTestAIController* CreateAIController(TSubclassOf<AController> AIControllerClass);

	virtual void RemoveBot();
	virtual void SpawnBot();

private:	

	int32 GetTeamsCount() const;
	void CollectPlayerStarts();

	UPROPERTY()
	TArray<AUnrealTestPlayerTeam*> Teams;
	int32 NextAutoTeamIndex = 0;

	mutable TWeakObjectPtr<AUnrealTestGameState> GameStatePtr;
};



