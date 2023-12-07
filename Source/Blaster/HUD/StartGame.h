// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StartGame.generated.h"


/**
 * 
 */
class UButton;
UCLASS()
class BLASTER_API UStartGame : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UButton* StartGameButton;

	virtual void NativeConstruct() override;

	UFUNCTION()
	void StartGameClicked();
	
};
