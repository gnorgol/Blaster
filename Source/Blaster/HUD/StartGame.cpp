// Fill out your copyright notice in the Description page of Project Settings.


#include "StartGame.h"
#include "Components/Button.h"
#include <Blaster/GameMode/LobbyGameMode.h>
#include "Blaster/PlayerController/BlasterPlayerController.h"


void UStartGame::NativeConstruct()
{
		Super::NativeConstruct();

	if (StartGameButton)
	{
		StartGameButton->OnClicked.AddDynamic(this, &ThisClass::StartGameClicked);
	}
}

void UStartGame::StartGameClicked()
{
		UE_LOG(LogTemp, Warning, TEXT("Start Game Clicked"));





		if (APlayerController* PC = GetOwningPlayer())
		{
			if (ALobbyGameMode* GM = Cast<ALobbyGameMode>(PC->GetWorld()->GetAuthGameMode()))
			{
				FInputModeGameOnly InputModeData;				
				PC->SetInputMode(InputModeData);
				PC->bShowMouseCursor = false;

				GM->ButtonStartGameClicked();
			}
		}
}
