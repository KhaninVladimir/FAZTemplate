// Copyright Epic Games, Inc. All Rights Reserved.

#include "Game/UnrealTestGameMode.h"
#include "Game/UnrealTestPlayerStart.h"
#include "Game/UnrealTestPlayerTeam.h"
#include "Character/UnrealTestCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/GameSession.h"
#include <EngineUtils.h>

#include "Player/UnrealTestPlayerController.h"
#include "Player/UnrealTestPlayerState.h"

#include "AI/UnrealTestAIController.h"

AUnrealTestGameMode::AUnrealTestGameMode()
{
	GameModePlayerStarts = AUnrealTestPlayerStart::StaticClass();
	SetActorTickInterval(1.f);
}

void AUnrealTestGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	CollectPlayerStarts();
	NextAutoTeamIndex = FMath::RandRange(0, GetTeamsCount() - 1);

	SpawnDefaultsTeams(GetTeamsCount());
}

APlayerController* AUnrealTestGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	APlayerController* PlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);

	if (auto PC = Cast<AUnrealTestPlayerController>(PlayerController))
	{
		ConnectedPlayers.Push(PC);
	}

	return PlayerController;
}

void AUnrealTestGameMode::Logout(AController* Exiting)
{
	if (auto PC = Cast<AUnrealTestPlayerController>(Exiting))
	{
		ConnectedPlayers.Remove(PC);
	}
	Super::Logout(Exiting);
}

FString AUnrealTestGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	SetDefaultPlayerTeam(NewPlayerController);

	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

void AUnrealTestGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void AUnrealTestGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!IsMatchInProgress())
		return;

	if (!GameSession)
		return;
	const int MaxPlayer = GameSession->MaxPlayers;
	const int PlayersCount = ConnectedPlayerNum();
	const int BotCount = AIControllers.Num();

	if (PlayersCount > (MaxPlayer - BotCount) || MaxBotCount < BotCount)
	{
		RemoveBot();
	}
	else if (PlayersCount < (MaxPlayer - BotCount) && MaxBotCount > BotCount)
	{
		SpawnBot();
	}
}

AUnrealTestPlayerTeam* AUnrealTestGameMode::SetDefaultPlayerTeam(const AController* Player)
{
	AUnrealTestPlayerTeam* Team = nullptr;
	if (Teams.Num() > 0)
	{
		NextAutoTeamIndex = (NextAutoTeamIndex + 1) % Teams.Num();
		Team = Teams[NextAutoTeamIndex];
	}
	AssignPlayerToTeam(Player, Team);
	return Team;
}

void AUnrealTestGameMode::AssignPlayerToTeam(const AController* Player, AUnrealTestPlayerTeam* Team)
{
	if (!Team)
		return;

	if (AUnrealTestPlayerState* PS = Player->GetPlayerState<AUnrealTestPlayerState>())
	{
		Team->AddMember(PS);
		PS->SetPlayerTeam(Team);
	}
}

void AUnrealTestGameMode::SpawnDefaultsTeams(int32 Count)
{
	UWorld* World = GetWorld();
	for (int32 TeamId = 0; TeamId < Count; ++TeamId)
	{
		AUnrealTestPlayerTeam* NewTeam = World->SpawnActorDeferred<AUnrealTestPlayerTeam>(
			AUnrealTestPlayerTeam::StaticClass(),
			FTransform::Identity,
			this,
			GetInstigator(),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (!NewTeam)
			break;

		const FString TeamName = TEXT("Team") + FString::FromInt(TeamId);
		NewTeam->SetTeamInfo(TeamId, TeamName);
		UGameplayStatics::FinishSpawningActor(NewTeam, FTransform::Identity);

		Teams.Push(NewTeam);
	}
}

AUnrealTestPlayerTeam* AUnrealTestGameMode::SpawnTeam(int32 TeamId)
{
	UWorld* World = GetWorld();
	AUnrealTestPlayerTeam* Team = World->SpawnActorDeferred<AUnrealTestPlayerTeam>(
		AUnrealTestPlayerTeam::StaticClass(),
		FTransform::Identity,
		this,
		GetInstigator(),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (!Team)
		return nullptr;

	const FString TeamName = TEXT("Team") + FString::FromInt(TeamId);
	Team->SetTeamInfo(TeamId, TeamName);
	UGameplayStatics::FinishSpawningActor(Team, FTransform::Identity);
	return Team;
}

void AUnrealTestGameMode::RemoveBot()
{
	TArray<AUnrealTestPlayerState*> PlayerStates;
	for (TActorIterator<AUnrealTestPlayerState> It(GetWorld()); It; ++It)
	{
		if (auto PS = *It)
		{
			if (PS->IsABot())
				PlayerStates.Add(PS);
		}
	}

	PlayerStates.Sort([&](const AUnrealTestPlayerState& PS_A, const AUnrealTestPlayerState& PS_B) {
		return PS_A.GetScore() < PS_B.GetScore();
	});

	if (!PlayerStates.IsValidIndex(0))
		return;

	auto PS = PlayerStates[0];
	return;

	auto AI = Cast<AUnrealTestAIController>(PS->GetOwner());
	if (!AI)
		return;

	DestroyAIController(AI);
}

void AUnrealTestGameMode::SpawnBot()
{
	if (!IsMatchInProgress())
		return;

	if (BotSpawnedClasses.Num() <= 0)
		return;
	
	TSubclassOf<AUnrealTestCharacter> BotTemplate = BotSpawnedClasses[rand() % BotSpawnedClasses.Num()];
	if (!BotTemplate)
		return;

	auto BotCDO = BotTemplate->GetDefaultObject<AUnrealTestCharacter>();
	if (!BotCDO)
		return;

	auto AI = CreateAIController(BotCDO->AIControllerClass);
	auto PS = Cast<AUnrealTestPlayerState>(AI->PlayerState);
	if (!PS)
		return;

	PS->SetPlayerPawn(BotTemplate);

	SetDefaultPlayerTeam(AI);
	RestartPlayer(AI);
}


AUnrealTestAIController* AUnrealTestGameMode::CreateAIController(TSubclassOf<AController> AIControllerClass)
{
	if (!AIControllerClass)
		return nullptr;

	auto SpawnedAIContoller = GetWorld()->SpawnActor<AUnrealTestAIController>(*AIControllerClass, FTransform());
	if (!SpawnedAIContoller)
		return nullptr;

	auto BotNumber = AIControllers.Num() + 1;
	ChangeName(SpawnedAIContoller, FString::Printf(TEXT("BOT_%d"), BotNumber), false);

	AIControllers.Add(SpawnedAIContoller);
	return SpawnedAIContoller;
}

void AUnrealTestGameMode::DestroyAIController(AController* AIController)
{
	auto AI = Cast<AUnrealTestAIController>(AIController);
	if (!AI)
		return;

	if (AIControllers.Remove(AI) <= 0)
		return;
	
	if (auto PlayerState = AI->GetPlayerState<APlayerState>())
	{
		PlayerState->Destroy();
	}

	auto Char = AI->GetPawn();
	if (Char)
	{
		Char->DetachFromControllerPendingDestroy();
		Char->Destroy();
	}

	AI->Destroy();
}

void AUnrealTestGameMode::RestartPlayer(AController* NewPlayer)
{
	if (NewPlayer->GetPawn())
	{
		NewPlayer->UnPossess();
	}

	APlayerController* PC = Cast<APlayerController>(NewPlayer);
	if (PC && !PlayerCanRestart(PC))
	{
		return;
	}

	Super::RestartPlayer(NewPlayer);
}

UClass* AUnrealTestGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	auto PS = Cast<AUnrealTestPlayerState>(InController->PlayerState);
	if (PS && PS->GetSelectedPawnClass())
		return PS->GetSelectedPawnClass();

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}

AActor* AUnrealTestGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	if (AUnrealTestPlayerStart* SelectedPoint = Cast<AUnrealTestPlayerStart>(Player->StartSpot))
	{
		Player->StartSpot = nullptr;
		return SelectedPoint;
	}

	TArray<AUnrealTestPlayerStart*> AvailableStarts = PlayerStarts;

	AUnrealTestPlayerState* PS = Cast<AUnrealTestPlayerState>(Player->PlayerState);
	AUnrealTestPlayerTeam* Team = PS ? PS->GetPlayerTeam() : nullptr;	

	if (!PS || !Team)
	{
		return AvailableStarts[rand() % AvailableStarts.Num()];
	}
	
	FGenericTeamId TeamId = Team->GetTeamID();

	AvailableStarts.RemoveAllSwap([&TeamId](AUnrealTestPlayerStart* Start)
	{
		return !(Start->IsReady() && Start->IsTeamPlayerStart(TeamId));
	});

	if (AvailableStarts.Num() > 0)
	{
		AUnrealTestPlayerStart* SelectedPoint = AvailableStarts[rand() % AvailableStarts.Num()];
		SelectedPoint->OccupyPoint();
		return SelectedPoint;
	}

	//Bad result, we always must have available PlayerStarts.
	//PIE will be Aborted, if PlayerStarts not enough for start
	return Super::FindPlayerStart_Implementation(Player, IncomingName);
}

bool AUnrealTestGameMode::PlayerCanRestart_Implementation(APlayerController* Player)
{
	bool bCanRestart = IsMatchInProgress();
	if (auto PS = Cast<AUnrealTestPlayerState>(Player->PlayerState))
	{
		bCanRestart &= PS->CanRespawn();
	}

	return bCanRestart;
}

int32 AUnrealTestGameMode::GetTeamsCount() const
{
	return 2;
}

void AUnrealTestGameMode::CollectPlayerStarts()
{
	if (!GameModePlayerStarts)
	{
		GameModePlayerStarts = APlayerStart::StaticClass();
	}

	for (TActorIterator<AUnrealTestPlayerStart> It(GetWorld(), GameModePlayerStarts); It; ++It)
	{
		PlayerStarts.Add(*It);
	}
}

void AUnrealTestGameMode::Killed(AController* Killer, AController* VictimPlayer, APawn* VictimPawn, AActor* DamageCauser)
{
#if UE_SERVER || WITH_EDITOR
	AUnrealTestPlayerState* VictimPS = VictimPlayer ? Cast<AUnrealTestPlayerState>(VictimPlayer->PlayerState) : nullptr;
	if (VictimPS)
	{
		VictimPS->AddDeathCount();
	}

	AUnrealTestPlayerState* KillerPS = Killer ? Cast<AUnrealTestPlayerState>(Killer->PlayerState) : nullptr;
	if (KillerPS && Killer != VictimPlayer)
	{
		KillerPS->AddKillCount();
	}

	if (auto Char = Cast<AUnrealTestCharacter>(VictimPawn))
	{
		for (auto PS : Char->AssistsList)
		{
			PS->AddAssistCount();
		}
	}
#endif
}