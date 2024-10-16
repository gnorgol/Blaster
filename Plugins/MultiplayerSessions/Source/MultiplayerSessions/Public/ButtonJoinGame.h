// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommonActivatableWidget.h"
#include "OnlineSessionSettings.h"
#include "ButtonJoinGame.generated.h"

/**
 * 
 */
class UButton;
class UTextBlock;
class FOnlineSessionSearchResult;
UCLASS()
class MULTIPLAYERSESSIONS_API UButtonJoinGame : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* JoinButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* HostNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* GameModeNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* MapNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* NumPlayersText;

	FOnlineSessionSearchResult SearchResult;

	class UMultiplayerSessionsSubsystem* MultiplayerSubsystem;

	UFUNCTION(BlueprintCallable)
	void JoinGame();

};
