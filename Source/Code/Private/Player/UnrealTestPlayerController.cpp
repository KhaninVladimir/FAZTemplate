// Copyright 2022 Khanin Vladimir (FAZIC)


#include "Player/UnrealTestPlayerController.h"
#include "Player/UnrealTestPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Game/UnrealTestGameMode.h"
#include "Game/UnrealTestGameState.h"

#include "Character/UnrealTestCharacter.h"

AUnrealTestPlayerController::AUnrealTestPlayerController()
{

}

void AUnrealTestPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction("OpenScoreMenu", IE_Pressed, this, &AUnrealTestPlayerController::OpenScoreMenu);
	InputComponent->BindAction("OpenScoreMenu", IE_Released, this, &AUnrealTestPlayerController::CloseScoreMenu);

	const int32 KeysCount = 9;
	const FKey KeysList[KeysCount] = { EKeys::One, EKeys::Two, EKeys::Three,
										EKeys::Four, EKeys::Five, EKeys::Six,
										EKeys::Seven, EKeys::Eight, EKeys::Nine };

	for (int32 i = 0; i < KeysCount; ++i)
	{
		FInputKeyBinding SelectCharacterKey(KeysList[i], IE_Pressed);
		SelectCharacterKey.KeyDelegate.GetDelegateForManualSet().BindLambda([this, i]()
		{
			UE_LOG(LogTemp, Warning, TEXT("YOI MARIO"));
			SelectCharacter(i);
		});
		InputComponent->KeyBindings.Emplace(MoveTemp(SelectCharacterKey));
	}
}

void AUnrealTestPlayerController::OpenScoreMenu()
{
	OnOpenScoreMenu.Broadcast(true);
}

void AUnrealTestPlayerController::CloseScoreMenu()
{
	OnOpenScoreMenu.Broadcast(false);
}


void AUnrealTestPlayerController::SelectCharacter(int32 Index)
{
	Server_SelectCharacter(Index);
}

void AUnrealTestPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	OnPlayerStateChangedClient.Broadcast(GetUTPlayerState());
}

bool AUnrealTestPlayerController::Server_SelectCharacter_Validate(int32 Index) { return true; }
void AUnrealTestPlayerController::Server_SelectCharacter_Implementation(int32 Index)
{
	if (auto PS = GetUTPlayerState())
	{
		PS->SetPlayerPawn(Index);
	}
}

void AUnrealTestPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	if (auto PS = GetUTPlayerState())
		PS->SetPlayerPhase(EPlayerPhase::PP_Battle);
}

void AUnrealTestPlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}

void AUnrealTestPlayerController::Client_SendKilledInfo_Implementation(const FUTKilledInfo& Data)
{
	Client_OnPlayerKilled.Broadcast(Data);
}

AUnrealTestPlayerState* AUnrealTestPlayerController::GetUTPlayerState() const
{
	if (!PlayerStatePtr.IsValid() && PlayerState)
	{
		PlayerStatePtr = Cast<AUnrealTestPlayerState>(PlayerState);
	}

	return PlayerStatePtr.Get();
}