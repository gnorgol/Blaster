// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatBox.h"
#include "Components/TextBlock.h"
#include "Components/EditableTextBox.h"
#include "Components/ScrollBox.h"

void UChatBox::AddChatMessage(const FText& Message, const FString& PlayerName)
{
	FString ChatMessage = FString::Printf(TEXT("%s: %s"), *PlayerName, *Message.ToString());
	
	// Add the message to the Chat Text
	FString ChatText = ChatTextBlock->GetText().ToString();
	ChatText += ChatMessage + "\n";
	ChatTextBlock->SetText(FText::FromString(ChatText));
	ChatScrollBox->ScrollToEnd();
	// Set visibility to true
	SetVisibility(ESlateVisibility::Visible);

	//Hide the chat box after 5 seconds
	GetWorld()->GetTimerManager().ClearTimer(ChatTimerHandle);
	if (!ChatInputText->HasKeyboardFocus())
	{
		GetWorld()->GetTimerManager().SetTimer(ChatTimerHandle, this, &UChatBox::HideChatBox, 5.0f, false);
	}

}

void UChatBox::HideChatBox()
{
	// if ChatInputText is not focused, hide the chat box
	if (!ChatInputText->HasKeyboardFocus())
	{
		SetVisibility(ESlateVisibility::Hidden);
	}
	
}

