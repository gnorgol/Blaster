// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterLobbyPlayerController.generated.h"


/**
 * 
 */

class UInputAction;
class UMenuInGame;
class UChatBox;
class ALobbyGameMode;



UCLASS()
class BLASTER_API ABlasterLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

	public:
		void OnPossess(APawn* InPawn) override;
		UFUNCTION(Client, Reliable)
		void ClientChatCommitted(const FText& Text, const FString& PlayerName);

protected:
	virtual void SetupInputComponent();
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MenuAction;
	void ShowRetunToMainMenu();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ChatAction;
	void ShowOrHideChat();
	bool bChatVisible = false;

	UPROPERTY()
	class UChatBox* ChatWidget;


private:
	UPROPERTY(EditAnywhere, Category = HUD)
	TSubclassOf<class UUserWidget> MenuInGameWidgetClass;

	UPROPERTY()
	class UMenuInGame* MenuInGameWidget;

	UPROPERTY(EditAnywhere, Category = HUD)
	TSubclassOf<UUserWidget> ChatWidgetClass;

	bool bChatWidgetVisible = false;

	bool bMenuInGameWidgetVisible = false;

	UFUNCTION()
	void OnChatMessageSent(const FText& Message, ETextCommit::Type CommitMethod);

	UFUNCTION(Server, Reliable)
	void ServerChatCommitted(const FText& Text, const FString& PlayerName);

	UPROPERTY()
	ALobbyGameMode* LobbyGameMode;


	
};
