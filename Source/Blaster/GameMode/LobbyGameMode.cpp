// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "MultiplayerSessionsSubsystem.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	int32 NumOfPlayers = GameState.Get()->PlayerArray.Num();

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UMultiplayerSessionsSubsystem* Subsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		check(Subsystem);
		if (NumOfPlayers == Subsystem->DesiredNumPublicConnections)
		{
			UWorld* World = GetWorld();
			if (!ensure(World != nullptr)) return;
			bUseSeamlessTravel = true;
			FString GameMode = Subsystem->DesiredGameMode;
			if (GameMode == "FreeForAll")
			{
				World->ServerTravel("/Game/Maps/Hangar?listen");
			}
			else if (GameMode == "TeamDeathMatch")
			{
				World->ServerTravel("/Game/Maps/TeamDeathMatch?listen");
			}

			
			
		}
	}


}
