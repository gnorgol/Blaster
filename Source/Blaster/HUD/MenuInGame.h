// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommonActivatableWidget.h"
#include <CommonButtonBase.h>
#include <Components/CanvasPanel.h>
#include <Components/Slider.h>
#include "Components/CheckBox.h"
#include "UpscaleMode.h"
#include "DLSSLibrary.h"
#include "NISLibrary.h"
#include "MenuInGame.generated.h"




/**
 * 
 */
class UMultiplayerSessionsSubsystem;
class APlayerController;
class UCanvasPanel;
class UVerticalBox;
class UHorizontalBox;
class UButton;
class UInputMappingContext;
class ABlasterCharacter;
class ASlider;
class UTextBlock;
class UComboBoxString;
class UCheckBox;


UCLASS()
class BLASTER_API UMenuInGame : public UCommonActivatableWidget
{
	GENERATED_BODY()
friend class UKeyMappingButton;
public:
	void MenuSetup();
	UFUNCTION(BlueprintCallable)
	void MenuTeardown();

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (BindWidget))
	UButton* SettingButton;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (BindWidget))
	UButton* GraphicSettingButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* MenuPanel;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* SettingPanel;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* GraphicSettingPanel;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* ContactMePanel;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UCanvasPanel* CreditsPanel;



protected:
	virtual bool Initialize() override;

	UFUNCTION()
		void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
		void OnPlayerLeftGame();


private:

	EUpscaleMode CurrentUpscaleMode;
	UPROPERTY(meta = (BindWidget))
		 UButton* ReturnMainMenuButton;




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
	UPROPERTY(meta = (BindWidget))
		UCheckBox* DLSSFGCheckBox;
		


	UPROPERTY(meta = (BindWidget))
	USlider* NISSharpnessSlider;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ValueNISSharpnessText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* UpscalingModeWarningText;


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
	UFUNCTION()
	void OnUpscalingModeComboBoxValueChanged(FString Value, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnDLSSModeComboBoxValueChanged(FString Value, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnNISModeComboBoxValueChanged(FString Value, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnNISSharpnessSliderValueChanged(float Value);
	//Event end NIS sharpness slider Mouse
	UFUNCTION()
	void OnNISSharpnessSliderMouseEnd();
	UFUNCTION()
	void OnDLSSFGCheckBoxChanged(bool bIsChecked);


	UPROPERTY()
		UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

	UPROPERTY()
		APlayerController* PlayerController;


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
	UPROPERTY(meta = (BindWidget))
		UComboBoxString* UpscalingModeComboBox;
	UPROPERTY(meta = (BindWidget))
		UComboBoxString* DLSSModeComboBox;
	UPROPERTY(meta = (BindWidget))
		UComboBoxString* NISModeComboBox;
	UPROPERTY(meta = (BindWidget))
		UHorizontalBox* DLSSModeBox;
	UPROPERTY(meta = (BindWidget))
		UHorizontalBox* NISModeBox;
	UPROPERTY(meta = (BindWidget))
		UHorizontalBox* NISSharpnessBox;
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* NvidiaFrameGenerationBox;
	UPROPERTY(meta = (BindWidget))
	UButton* LinkedinLogoButton;
	UPROPERTY(meta = (BindWidget))
	UButton* DeadghostInteractiveEpicGamesLogoButton;

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

	UPROPERTY(EditAnywhere, Category = Input)
	TSubclassOf<class UCommonButtonBase> ExitButtonClass;

	UPROPERTY()
		ABlasterCharacter* BlasterCharacter;

	UPROPERTY()
	TArray<FString> DisplayList;

	bool IsWindowOnMonitor(FVector2D WindowPosition, FMonitorInfo MonitorInfo);

	void SetUpscaleDefault();
	void SetUpscaleDLSS();
	void SetUpscaleNIS();

	void SetDLSSMode(UDLSSMode DLSSMode);

	void ShowDLSSModeBox(bool bShow);
	void ShowNISModeBox(bool bShow);

	void SetWarningText(FString WarningText);


};
