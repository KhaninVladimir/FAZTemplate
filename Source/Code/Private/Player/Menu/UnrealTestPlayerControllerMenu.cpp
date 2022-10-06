// Copyright 2022 Khanin Vladimir (FAZIC)


#include "Player/Menu/UnrealTestPlayerControllerMenu.h"
#include "OnlineSubsystemUtils.h"
#include "Game/UnrealTestGameSession.h"
#include "Game/UTOnlineGameSetting.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/GameModeBase.h"

void AUnrealTestPlayerControllerMenu::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}


AUnrealTestGameSession* AUnrealTestPlayerControllerMenu::GetGameSession() const
{
	UWorld* const World = GetWorld();
	if (!World)
		return nullptr;
	
	AGameModeBase* const GM = World->GetAuthGameMode();
	if (!GM)
		return nullptr;
	
	return Cast<AUnrealTestGameSession>(GM->GameSession);
}