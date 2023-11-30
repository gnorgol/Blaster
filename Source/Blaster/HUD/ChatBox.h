// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatBox.generated.h"

/**
 * 
 */
class UTextBlock;
class UEditableTextBox;
class UScrollBox;
UCLASS()
class BLASTER_API UChatBox : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	UScrollBox* ChatScrollBox;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ChatTextBlock;
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* ChatInputText;
	void AddChatMessage(const FText& Message, const FString& PlayerName);

	FTimerHandle ChatTimerHandle;

	void HideChatBox();
};
