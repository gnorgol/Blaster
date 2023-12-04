// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
class UMultiplayerSessionsSubsystem;
UCLASS()
class BLASTER_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	void TravelToMap(UMultiplayerSessionsSubsystem* Subsystem, bool& retFlag);

	void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = HUD)
	TSubclassOf<UUserWidget> ButtonStartGameClass;

	void ButtonStartGameClicked();

	void SendChatMsg(const FText& Text, const FString& PlayerName);
};
