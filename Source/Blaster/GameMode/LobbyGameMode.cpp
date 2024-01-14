// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include <Blaster/HUD/StartGame.h>
#include <Blaster/Character/BlasterCharacter.h>
#include <Blaster/PlayerController/BlasterLobbyPlayerController.h>
#include "OnlineSessionSettings.h"
#include <Kismet/GameplayStatics.h>
#include "GameFramework/PlayerState.h"


void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	int32 NumOfPlayers = GameState.Get()->PlayerArray.Num();

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UMultiplayerSessionsSubsystem* Subsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		check(Subsystem);
		Subsystem->SetNumOfPlayersInSession(NumOfPlayers);



		if (NumOfPlayers == Subsystem->DesiredNumPublicConnections)
		{
			bool retFlag;
			TravelToMap(Subsystem, retFlag);
			if (retFlag) return;
			
		}
	}


}

void ALobbyGameMode::RemovePlayerFromLobby(APlayerController* LeavingPlayer)
{
	ABlasterLobbyPlayerController* BlasterLobbyPlayerController = Cast<ABlasterLobbyPlayerController>(LeavingPlayer);
	GameState.Get()->PlayerArray.Remove(BlasterLobbyPlayerController->PlayerState);
	int32 NumOfPlayers = GameState.Get()->PlayerArray.Num();
	UGameInstance* GameInstance = GetGameInstance();


	if (GameInstance)
	{
		UMultiplayerSessionsSubsystem* Subsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		check(Subsystem);
		int32 LocalUserNum = LeavingPlayer->PlayerState->GetPlayerId();
		Subsystem->RemovePlayerFromSession(LocalUserNum, NAME_GameSession, *LeavingPlayer->PlayerState->GetUniqueId());
		//Get named session
		Subsystem->UnregisterPlayerFromSession(NAME_GameSession, *LeavingPlayer->PlayerState->GetUniqueId());
		Subsystem->SetNumOfPlayersInSession(NumOfPlayers);
	}
	//destroy BlasterCharacter
	BlasterLobbyPlayerController->GetPawn()->Destroy();
	BlasterLobbyPlayerController->Destroy();
}

void ALobbyGameMode::TravelToMap(UMultiplayerSessionsSubsystem* Subsystem, bool& retFlag)
{
	retFlag = true;
	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;
	bUseSeamlessTravel = false;
	FString GameMode = Subsystem->DesiredGameMode;
	FString MapName = Subsystem->DesiredMapName;

	FString URL = FString::Printf(TEXT("/Game/Maps/%s%s?listen"), *MapName, *GameMode);
	World->ServerTravel(URL);
	retFlag = false;
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		UStartGame* ButtonStartGame = CreateWidget<UStartGame>(PlayerController, ButtonStartGameClass);
		if (ButtonStartGame)
		{
			ButtonStartGame->AddToViewport(1);
		}
	}
}


void ALobbyGameMode::ButtonStartGameClicked()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (!ensure(GameInstance != nullptr)) return;
	UMultiplayerSessionsSubsystem* Subsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	check(Subsystem);
	bool retFlag;
	TravelToMap(Subsystem, retFlag);
	if (retFlag) return;


}

void ALobbyGameMode::SendChatMsg(const FText& Text, const FString& PlayerName)
{
    UWorld* World = GetWorld();
    if (!ensure(World != nullptr)) return;

    for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
    {
        ABlasterLobbyPlayerController* BlasterLobbyPlayerController = Cast<ABlasterLobbyPlayerController>(*It);
        if (BlasterLobbyPlayerController && BlasterLobbyPlayerController->IsValidLowLevel())
        {
            BlasterLobbyPlayerController->ClientChatCommitted(Text, PlayerName);
        }
    }
}
