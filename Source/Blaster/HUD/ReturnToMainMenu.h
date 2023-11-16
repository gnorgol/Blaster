// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReturnToMainMenu.generated.h"

/**
 * 
 */
class UMultiplayerSessionsSubsystem;
class APlayerController;
UCLASS()
class BLASTER_API UReturnToMainMenu : public UUserWidget
{
	GENERATED_BODY()
	public:
		void MenuSetup();
		void MenuTeardown();

protected:
	virtual bool Initialize() override;

	UFUNCTION()
		void OnDestroySessionComplete(bool bWasSuccessful);

private:
	UPROPERTY(meta = (BindWidget))
		class UButton* ReturnButton;

	UFUNCTION()
		void ReturnButtonClicked();

	UPROPERTY()
		UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY()
		APlayerController* PlayerController;
};
