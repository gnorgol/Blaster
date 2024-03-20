// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include <Components/CanvasPanel.h>
#include <Components/Slider.h>
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
class ASlider;
class UTextBlock;
class UComboBoxString;

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
		UButton* GraphicSettingButton;

	UPROPERTY(meta = (BindWidget))
		UButton* ContactMeButton;
	UPROPERTY(meta = (BindWidget))
	UButton* CreditsButton;
	UPROPERTY(meta = (BindWidget))
		UButton* ResetDefaultButton;

	UPROPERTY(meta = (BindWidget))
		USlider* MouseSensitivitySlider;
	UPROPERTY(meta = (BindWidget))
		USlider* AimSensitivitySlider;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* ValueMouseSensitivityText;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* ValueAimSensitivityText;


	UFUNCTION()
		void ReturnButtonClicked();
	UFUNCTION()
		void SettingButtonClicked();
	UFUNCTION()
		void GraphicSettingButtonClicked();
		void UpdateDisplayResolutionComboBox();
	UFUNCTION()
		void ResetDefaultButtonClicked();
	UFUNCTION()
		void ContactMeButtonClicked();
	UFUNCTION()
		void CreditsButtonClicked();

	UFUNCTION()
		void OnMouseSensitivityValueChanged(float Value);
	UFUNCTION()
		void OnAimSensitivityValueChanged(float Value);
	UFUNCTION()
		void OnQualityComboBoxValueChanged(FString Value, ESelectInfo::Type SelectionType);
	UFUNCTION()
		void OnDisplayModeComboBoxValueChanged(FString Value, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnDisplayResolutionComboBoxValueChanged(FString Value, ESelectInfo::Type SelectionType);
	UFUNCTION()
		void OnDisplayMonitorComboBoxValueChanged(FString Value, ESelectInfo::Type SelectionType);

	UPROPERTY()
		UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY()
		APlayerController* PlayerController;

	UPROPERTY(meta = (BindWidget))
		UCanvasPanel* MenuPanel;
	UPROPERTY(meta = (BindWidget))
		UCanvasPanel* SettingPanel;
	UPROPERTY(meta = (BindWidget))
		UCanvasPanel* GraphicSettingPanel;
	UPROPERTY(meta = (BindWidget))
		UCanvasPanel* ContactMePanel;
	UPROPERTY(meta = (BindWidget))
			UCanvasPanel* CreditsPanel;
	UPROPERTY(meta = (BindWidget))
		UVerticalBox* SettingBox;
	UPROPERTY(meta = (BindWidget))
		UComboBoxString* QualityComboBox;
	UPROPERTY(meta = (BindWidget))
		UComboBoxString* DisplayModeComboBox;
	UPROPERTY(meta = (BindWidget))
		UComboBoxString* DisplayResolutionComboBox;
	UPROPERTY(meta = (BindWidget))
		UComboBoxString* DisplayMonitorComboBox;



	void ClearSettingBox();

	UFUNCTION(BlueprintCallable)
		void ShowMenuPanel(ESlateVisibility bShow);
	UFUNCTION(BlueprintCallable)
		void ShowSettingPanel(ESlateVisibility bShow);
	UFUNCTION(BlueprintCallable)
	void ShowContactMePanel(ESlateVisibility bShow);
	UFUNCTION(BlueprintCallable)
		void ShowCreditsPanel(ESlateVisibility bShow);
	UFUNCTION(BlueprintCallable)
	void ShowGraphicSettingPanel(ESlateVisibility bShow);

	UPROPERTY(EditAnywhere, Category = HUD)
		TSubclassOf<class UUserWidget> KeyMappingButtonClass;

	UPROPERTY()
		ABlasterCharacter* BlasterCharacter;

	UPROPERTY()
	TArray<FString> DisplayList;

	bool IsWindowOnMonitor(FVector2D WindowPosition, FMonitorInfo MonitorInfo);

};
