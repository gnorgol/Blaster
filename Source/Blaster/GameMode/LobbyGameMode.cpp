// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	int32 NumOfPlayers = GameState.Get()->PlayerArray.Num();

	if (NumOfPlayers == 2)
	{
		UWorld* World = GetWorld();
		if (!ensure(World != nullptr)) return;

		bUseSeamlessTravel = true;
		World->ServerTravel("/Game/Maps/Hangar?listen");
	}
}
