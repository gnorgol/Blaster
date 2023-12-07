// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/CanvasPanel.h>
#include "MenuInGame.generated.h"

/**
 * 
 */
class UMultiplayerSessionsSubsystem;
class APlayerController;
class UCanvasPanel;
class UVerticalBox;
class UButton;
class UInputMappingContext;
class ABlasterCharacter;
UCLASS()
class BLASTER_API UMenuInGame : public UUserWidget
{
	GENERATED_BODY()
friend class UKeyMappingButton;
public:
	void MenuSetup();
	void MenuTeardown();

protected:
	virtual bool Initialize() override;

	UFUNCTION()
		void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
		void OnPlayerLeftGame();

private:
	UPROPERTY(meta = (BindWidget))
		 UButton* ReturnMainMenuButton;
	UPROPERTY(meta = (BindWidget))
		UButton* SettingButton;
	UPROPERTY(meta = (BindWidget))
		UButton* ContactMeButton;
	UPROPERTY(meta = (BindWidget))
	UButton* CreditsButton;
	UFUNCTION()
		void ReturnButtonClicked();
	UFUNCTION()
		void SettingButtonClicked();
	UFUNCTION()
		void ContactMeButtonClicked();
	UFUNCTION()
		void CreditsButtonClicked();

	UPROPERTY()
		UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY()
		APlayerController* PlayerController;

	UPROPERTY(meta = (BindWidget))
		UCanvasPanel* MenuPanel;
	UPROPERTY(meta = (BindWidget))
		UCanvasPanel* SettingPanel;
	UPROPERTY(meta = (BindWidget))
		UCanvasPanel* ContactMePanel;
	UPROPERTY(meta = (BindWidget))
			UCanvasPanel* CreditsPanel;
	UPROPERTY(meta = (BindWidget))
		UVerticalBox* SettingBox;


	void ClearSettingBox();

	UFUNCTION(BlueprintCallable)
		void ShowMenuPanel(ESlateVisibility bShow);
	UFUNCTION(BlueprintCallable)
		void ShowSettingPanel(ESlateVisibility bShow);
	UFUNCTION(BlueprintCallable)
	void ShowContactMePanel(ESlateVisibility bShow);
	UFUNCTION(BlueprintCallable)
		void ShowCreditsPanel(ESlateVisibility bShow);

	UPROPERTY(EditAnywhere, Category = HUD)
		TSubclassOf<class UUserWidget> KeyMappingButtonClass;

	UPROPERTY()
		ABlasterCharacter* BlasterCharacter;

};
