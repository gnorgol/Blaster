// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterLobbyPlayerController.h"
#include <Blaster/Character/BlasterCharacter.h>
#include <EnhancedInputComponent.h>
#include "Blaster/HUD/MenuInGame.h"
#include <Blaster/HUD/ChatBox.h>
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"
#include <Kismet/GameplayStatics.h>
#include <Blaster/GameMode/LobbyGameMode.h>
#include "GameFramework/PlayerState.h"


void ABlasterLobbyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);

}

void ABlasterLobbyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent == nullptr) return;
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Completed, this, &ABlasterLobbyPlayerController::ShowRetunToMainMenu);
		EnhancedInputComponent->BindAction(ChatAction, ETriggerEvent::Completed, this, &ABlasterLobbyPlayerController::ShowOrHideChat);
	}
}

void ABlasterLobbyPlayerController::BeginPlay()
{
	if (ChatWidgetClass)
	{
		APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		ChatWidget = CreateWidget<UChatBox>(LocalPlayerController, ChatWidgetClass);

		if (ChatWidget)
		{
			ChatWidget->AddToViewport();
			ChatWidget->ChatInputText->OnTextCommitted.AddDynamic(this, &ABlasterLobbyPlayerController::OnChatMessageSent);
			ChatWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABlasterLobbyPlayerController::ShowRetunToMainMenu()
{
	if (MenuInGameWidgetClass == nullptr)
	{
		return;
	}
	if (MenuInGameWidget == nullptr)
	{
		MenuInGameWidget = CreateWidget<UMenuInGame>(this, MenuInGameWidgetClass);
	}
	if (MenuInGameWidget)
	{
		bMenuInGameWidgetVisible = !bMenuInGameWidgetVisible;
		if (bMenuInGameWidgetVisible)
		{
			MenuInGameWidget->MenuSetup();
		}
		else
		{
			MenuInGameWidget->MenuTeardown();
		}
	}
}

void ABlasterLobbyPlayerController::ShowOrHideChat()
{
	UE_LOG(LogTemp, Warning, TEXT("ShowOrHideChat"));
	if (ChatWidget)
	{
		bChatWidgetVisible = !bChatWidgetVisible;
		if (bChatWidgetVisible)
		{
			FInputModeGameAndUI InputModeGameAndUI;
			UE_LOG(LogTemp, Warning, TEXT("Show Chat"));
			ChatWidget->SetVisibility(ESlateVisibility::Visible);
			//Enter to ChatInputText
			ChatWidget->ChatInputText->SetUserFocus(GetWorld()->GetFirstPlayerController());
			SetInputMode(InputModeGameAndUI);
			SetShowMouseCursor(true);
			ChatWidget->ChatScrollBox->ScrollToEnd();


		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Hide Chat"));
			FInputModeGameOnly InputModeGameOnly;
			ChatWidget->SetVisibility(ESlateVisibility::Hidden);
			//Leave from ChatInputText
			SetInputMode(InputModeGameOnly);
			SetShowMouseCursor(false);

		}
	}
}

void ABlasterLobbyPlayerController::OnChatMessageSent(const FText& Message, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		FString PlayerName = GetPlayerState<APlayerState>()->GetPlayerName();

		ServerChatCommitted(Message, PlayerName);

		//Clear ChatInputText
		ChatWidget->ChatInputText->SetText(FText());
		//Leave from ChatInputText
		ShowOrHideChat();
		
	}
}

void ABlasterLobbyPlayerController::ClientChatCommitted_Implementation(const FText& Text, const FString& PlayerName)
{
	if (ChatWidget)
	{
		ChatWidget->AddChatMessage(Text, PlayerName);
	}
}

void ABlasterLobbyPlayerController::ServerChatCommitted_Implementation(const FText& Text, const FString& PlayerName)
{
	LobbyGameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
	if (LobbyGameMode)
	{
		LobbyGameMode->SendChatMsg(Text, PlayerName);
	}
}
