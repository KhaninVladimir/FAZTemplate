// Copyright 2022 Khanin Vladimir (FAZIC)


#include "UI/UTPlayerScoreListUW.h"

#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

TArray<AUnrealTestPlayerState*> UUTPlayerScoreListUW::GetSortedPlayerStates(AUnrealTestPlayerState*& LocalPlayer) const
{
	AGameStateBase* GS = UGameplayStatics::GetGameState(GetWorld());
	if (!GS)
	{
		return {};
	}

	APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	TArray<AUnrealTestPlayerState*> Players;

	for (APlayerState* PlayerState : GS->PlayerArray)
	{
		if (AUnrealTestPlayerState* PS = Cast<AUnrealTestPlayerState>(PlayerState))
		{
			if (IsValid(PS))
			{
				Players.Add(PS);
				if (PS == LocalPlayerController->PlayerState)
				{
					LocalPlayer = PS;
				}
			}
		}
	}

	Players.Sort([](const AUnrealTestPlayerState& A, const AUnrealTestPlayerState& B)
	{
		if (A.GetKillCount() != B.GetKillCount())
		{
			return A.GetKillCount() > B.GetKillCount();
		}

		return A.GetDeathCount() < B.GetDeathCount();
	});

	Players.SetNum(PlayersToShow);
	if (!Players.Contains(LocalPlayer))
	{
		Players.Add(LocalPlayer);
	}

	return Players;
}
