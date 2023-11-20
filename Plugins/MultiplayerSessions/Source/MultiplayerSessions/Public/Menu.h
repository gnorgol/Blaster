// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include <Components/CanvasPanel.h>
#include <Components/TextBlock.h>
#include <ButtonJoinGame.h>
#include <Components/VerticalBox.h>
#include "OnlineSessionSettings.h"
#include "Menu.generated.h"




/**
 * 
 */
class UButton;
class UCanvasPanel;
class UTextBlock;
class UButtonJoinGame;
class UVerticalBox;
class FOnlineSessionSearchResult;
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		void MenuSetup(int32 NumberOfPublicConnections = 4 , FString TypeOfMatch = TEXT("FreeForAll"), FString LobbyPath = TEXT("/Game/ThirdPerson/Maps/Lobby"));

protected:
	virtual bool Initialize() override;
	virtual void NativeDestruct() override;

	// Callbacks for the custom delegates
	UFUNCTION()
		void OnCreateSession(bool bWasSuccessful);

	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
		void OnStartSession(bool bWasSuccessful);
	UFUNCTION()
		void OnDestroySession(bool bWasSuccessful);
private:

	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;
	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* MainMenuPanel;
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* JoinMenuPanel;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* InfoJoinText;
	UFUNCTION(BlueprintCallable)
	void ShowJoinMenu(ESlateVisibility bShow);
	UFUNCTION(BlueprintCallable)
	void ShowMainMenu(ESlateVisibility bShow);
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* GamesBox;


	UPROPERTY(EditAnywhere, Category = HUD)
		TSubclassOf<class UUserWidget> ButtonJoinGameClass;




	void ClearGamesBox();


	UFUNCTION()
	void HostButtonClicked();
	UFUNCTION()
	void JoinButtonClicked();

	void MenuTeardown();

	//The Subsystem that will be used to create the session
	class UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int32 MaxNumPlayers{ 4 };
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FString GameMode {TEXT("FreeForAll")};

	FString PathToLobbyMap{ TEXT("") };
};
