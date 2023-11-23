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
	UFUNCTION()
		void ReturnButtonClicked();
	UFUNCTION()
		void SettingButtonClicked();

	UPROPERTY()
		UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY()
		APlayerController* PlayerController;

	UPROPERTY(meta = (BindWidget))
		UCanvasPanel* MenuPanel;
	UPROPERTY(meta = (BindWidget))
		UCanvasPanel* SettingPanel;
	UPROPERTY(meta = (BindWidget))
		UVerticalBox* SettingBox;

	void ClearSettingBox();

	UFUNCTION(BlueprintCallable)
		void ShowMenuPanel(ESlateVisibility bShow);
	UFUNCTION(BlueprintCallable)
		void ShowSettingPanel(ESlateVisibility bShow);

	UPROPERTY(EditAnywhere, Category = HUD)
		TSubclassOf<class UUserWidget> KeyMappingButtonClass;

	UPROPERTY()
		ABlasterCharacter* BlasterCharacter;


	//KeyMappingButton->InputKey1->BindOnKeySelected(FOnKeySelected::CreateUObject(this, &UMenuInGame::OnKeySelected, index));

};
