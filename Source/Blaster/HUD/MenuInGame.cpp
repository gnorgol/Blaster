// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuInGame.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include <Blaster/Character/BlasterCharacter.h>
#include "InputMappingContext.h"
#include "KeyMappingButton.h"
#include "Components/InputKeySelector.h"
#include <Components/TextBlock.h>
#include <Components/VerticalBox.h>
#include <Components/ComboBoxString.h>
#include <Components/Slider.h>
#include "PlayerMappableKeySettings.h"
#include <EnhancedInputSubsystems.h>
#include <Blaster/PlayerController/SaveInputMapping.h>
#include <Blaster/PlayerController/SaveSensitivity.h>
#include <Kismet/GameplayStatics.h>
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOMINMAX
#include <windows.h>
#include "GameFramework/GameUserSettings.h"
#include <Engine/GameEngine.h>
#include "Kismet/KismetSystemLibrary.h"



void UMenuInGame::MenuSetup()
{
    AddToViewport();
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable = true;

    UWorld* World = GetWorld();
    if (World)
    {
        PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
        if (PlayerController)
        {
            FInputModeGameAndUI InputModeData;
            InputModeData.SetWidgetToFocus(TakeWidget());
            PlayerController->SetInputMode(InputModeData);
            PlayerController->bShowMouseCursor = true;
            BlasterCharacter = Cast<ABlasterCharacter>(GetOwningPlayerPawn());
            if (BlasterCharacter)
            {
                BlasterCharacter->bDisableGameplayInput = true;
            }
        }
    }
    if (ReturnMainMenuButton && !ReturnMainMenuButton->OnClicked.IsBound())
    {
        ReturnMainMenuButton->OnClicked.AddDynamic(this, &UMenuInGame::ReturnButtonClicked);
    }
    if (SettingButton && !SettingButton->OnClicked.IsBound())
    {
        SettingButton->OnClicked.AddDynamic(this, &UMenuInGame::SettingButtonClicked);
    }
    if (GraphicSettingButton && !GraphicSettingButton->OnClicked.IsBound())
    {
        GraphicSettingButton->OnClicked.AddDynamic(this, &UMenuInGame::GraphicSettingButtonClicked);
    }
    if (ContactMeButton && !ContactMeButton->OnClicked.IsBound())
    {
        ContactMeButton->OnClicked.AddDynamic(this, &UMenuInGame::ContactMeButtonClicked);
    }
    if (CreditsButton && !CreditsButton->OnClicked.IsBound())
    {
        CreditsButton->OnClicked.AddDynamic(this, &UMenuInGame::CreditsButtonClicked);
    }
    if (ResetDefaultButton && !ResetDefaultButton->OnClicked.IsBound())
    {
        ResetDefaultButton->OnClicked.AddDynamic(this, &UMenuInGame::ResetDefaultButtonClicked);
    }
    if (MouseSensitivitySlider && !MouseSensitivitySlider->OnValueChanged.IsBound())
    {
        MouseSensitivitySlider->OnValueChanged.AddDynamic(this, &UMenuInGame::OnMouseSensitivityValueChanged);
    }
    if (AimSensitivitySlider && !AimSensitivitySlider->OnValueChanged.IsBound())
    {
        AimSensitivitySlider->OnValueChanged.AddDynamic(this, &UMenuInGame::OnAimSensitivityValueChanged);
    }
    if (QualityComboBox && !QualityComboBox->OnSelectionChanged.IsBound())
    {
        QualityComboBox->OnSelectionChanged.AddDynamic(this, &UMenuInGame::OnQualityComboBoxValueChanged);
    }
	if (DisplayModeComboBox && !DisplayModeComboBox->OnSelectionChanged.IsBound())
	{
		DisplayModeComboBox->OnSelectionChanged.AddDynamic(this, &UMenuInGame::OnDisplayModeComboBoxValueChanged);
	}
	if (DisplayResolutionComboBox && !DisplayResolutionComboBox->OnSelectionChanged.IsBound())
	{
		DisplayResolutionComboBox->OnSelectionChanged.AddDynamic(this, &UMenuInGame::OnDisplayResolutionComboBoxValueChanged);
	}

    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
        if (MultiplayerSessionsSubsystem && !MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.IsAlreadyBound(this, &UMenuInGame::OnDestroySession))
        {
            MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UMenuInGame::OnDestroySession);
        }
    }
}

void UMenuInGame::MenuTeardown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();

	if (World)
	{
		PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
		if (PlayerController)
		{
			FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->bShowMouseCursor = false;
			ABlasterCharacter* BlastCharacter = Cast<ABlasterCharacter>(GetOwningPlayerPawn());
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				USaveInputMapping* SaveGameInstance = Cast<USaveInputMapping>(UGameplayStatics::CreateSaveGameObject(USaveInputMapping::StaticClass()));

				SaveGameInstance = Cast<USaveInputMapping>(UGameplayStatics::LoadGameFromSlot(TEXT("BlasterInputMapping"), 0));

				if (SaveGameInstance && !SaveGameInstance->EnhancedActionMappings.IsEmpty())
				{
					//No need to save the mapping context, it is already saved in the save game instance
					BlastCharacter->BlastCharacterMappingContext->UnmapAll();
					BlastCharacter->BlastCharacterMappingContext->Mappings = SaveGameInstance->EnhancedActionMappings;

				}
				else
				{
					//Save the mapping context
					SaveGameInstance = Cast<USaveInputMapping>(UGameplayStatics::CreateSaveGameObject(USaveInputMapping::StaticClass()));
					SaveGameInstance->EnhancedActionMappings = BlastCharacter->BlastCharacterMappingContext->GetMappings();
					Subsystem->AddMappingContext(BlastCharacter->BlastCharacterMappingContext, 0);

				}
			}
			
			if (BlastCharacter)
			{
				BlastCharacter->bDisableGameplayInput = false;
			}
		}
	}
	if (ReturnMainMenuButton && ReturnMainMenuButton->OnClicked.IsBound())
	{
		ReturnMainMenuButton->OnClicked.RemoveDynamic(this, &UMenuInGame::ReturnButtonClicked);
	}
	if (SettingButton && SettingButton->OnClicked.IsBound())
	{
		SettingButton->OnClicked.RemoveDynamic(this, &UMenuInGame::SettingButtonClicked);
	}
	ShowMenuPanel(ESlateVisibility::Visible);
	ShowSettingPanel(ESlateVisibility::Hidden);
	ShowContactMePanel(ESlateVisibility::Hidden);
	ShowCreditsPanel(ESlateVisibility::Hidden);
	ShowGraphicSettingPanel(ESlateVisibility::Hidden);

	if (MultiplayerSessionsSubsystem && MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.IsAlreadyBound(this, &UMenuInGame::OnDestroySession))
	{
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.RemoveDynamic(this, &UMenuInGame::OnDestroySession);
	}

}

bool UMenuInGame::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}



	return true;
}

void UMenuInGame::OnDestroySession(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		ReturnMainMenuButton->SetIsEnabled(true);
		return;
	}
	UWorld* World = GetWorld();
	if (World)
	{
		AGameModeBase* GameMode = World->GetAuthGameMode<AGameModeBase>();
		if (GameMode)
		{
			GameMode->ReturnToMainMenuHost();
		}
		else
		{
			PlayerController = PlayerController == nullptr ? World->GetFirstPlayerController() : PlayerController;
			if (PlayerController)
			{
				PlayerController->ClientReturnToMainMenuWithTextReason(FText());
			}
		}
	}
}

void UMenuInGame::OnPlayerLeftGame()
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->DestroySession();
	}
}

void UMenuInGame::ReturnButtonClicked()
{
	ReturnMainMenuButton->SetIsEnabled(false);

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* FirstPlayerController = World->GetFirstPlayerController();

		if (BlasterCharacter)
		{
			BlasterCharacter->OnLeftGame.AddDynamic(this, &UMenuInGame::OnPlayerLeftGame);
			BlasterCharacter->ServerLeaveGame();			
		}
		else
		{
			ReturnMainMenuButton->SetIsEnabled(true);
		}
		
	}
	else
	{
		ReturnMainMenuButton->SetIsEnabled(true);
	}
}

void UMenuInGame::SettingButtonClicked()
{
	ShowMenuPanel(ESlateVisibility::Hidden);
	ShowSettingPanel(ESlateVisibility::Visible);
	int32 index = 0;

	if (BlasterCharacter)
	{
		for (const FEnhancedActionKeyMapping Mapping : BlasterCharacter->BlastCharacterMappingContext->GetMappings())
		{
			UKeyMappingButton* KeyMappingButton = CreateWidget<UKeyMappingButton>(this, KeyMappingButtonClass);
			if (KeyMappingButton)
			{
				UPlayerMappableKeySettings* PlayerMappableKeySettings = Mapping.GetPlayerMappableKeySettings();
				if (PlayerMappableKeySettings)
				{
					FText ActionName = Mapping.GetPlayerMappableKeySettings()->DisplayName;
					KeyMappingButton->InputKey1->SetSelectedKey(Mapping.Key);
					KeyMappingButton->KeyLabelText->SetText(ActionName);
					KeyMappingButton->AddToViewport();
					SettingBox->AddChild(KeyMappingButton);
					KeyMappingButton->KeyIndex = index;
				}

			}
			index++;

		}
		float MouseSensitivity = BlasterCharacter->GetSensitivity();
		float AimSensitivity = BlasterCharacter->GetAimSensitivity();
		if (MouseSensitivitySlider)
		{
			MouseSensitivitySlider->SetValue(MouseSensitivity);
		}
		if (AimSensitivitySlider)
		{
			AimSensitivitySlider->SetValue(AimSensitivity);
		}
		//Set Value Text
		if (ValueMouseSensitivityText)
		{
			ValueMouseSensitivityText->SetText(FText::AsNumber(MouseSensitivity));
		}
		if (ValueAimSensitivityText)
		{
			ValueAimSensitivityText->SetText(FText::AsNumber(AimSensitivity));
		}
		
	}
	
}

void UMenuInGame::GraphicSettingButtonClicked()
{
	ShowMenuPanel(ESlateVisibility::Hidden);
	ShowGraphicSettingPanel(ESlateVisibility::Visible);
	//Get current Material Quality
	if (QualityComboBox)
	{
		int32 CurrentQuality = UGameUserSettings::GetGameUserSettings()->ScalabilityQuality.GetSingleQualityLevel();
		QualityComboBox->SetSelectedIndex(CurrentQuality);
	}
	//Set Display Mode ComboBox
	if (DisplayModeComboBox)
	{
		EWindowMode::Type CurrentDisplayMode = UGameUserSettings::GetGameUserSettings()->GetFullscreenMode();
		switch (CurrentDisplayMode)
		{
			case EWindowMode::Fullscreen:
			DisplayModeComboBox->SetSelectedIndex(0);
			break;
			case EWindowMode::WindowedFullscreen:
				DisplayModeComboBox->SetSelectedIndex(1);
				break;
			case EWindowMode::Windowed:
				DisplayModeComboBox->SetSelectedIndex(2);
			break;

		}
	}
	//Set Display Resolution
	UpdateDisplayResolutionComboBox();


}

void UMenuInGame::UpdateDisplayResolutionComboBox()
{
	if (DisplayResolutionComboBox)
	{
		FIntPoint CurrentDisplayResolution = UGameUserSettings::GetGameUserSettings()->GetScreenResolution();
		FString CurrentDisplayResolutionString = FString::Printf(TEXT("%d x %d"), CurrentDisplayResolution.X, CurrentDisplayResolution.Y);
		TArray<FIntPoint> Resolutions;
		UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
		DisplayResolutionComboBox->ClearOptions();
		for (auto It = Resolutions.rbegin(); It != Resolutions.rend(); ++It)
		{
			FIntPoint Resolution = *It;
			FString ResolutionString = FString::Printf(TEXT("%d x %d"), Resolution.X, Resolution.Y);
			DisplayResolutionComboBox->AddOption(ResolutionString);
		}
		DisplayResolutionComboBox->SetSelectedOption(CurrentDisplayResolutionString);
	}
}

void UMenuInGame::ResetDefaultButtonClicked()
{
	int32 index = 0;
	if (BlasterCharacter)
	{
		//Detect if the keyborad is an AZERTY keyboard or a QWERTY keyboard
		switch (PRIMARYLANGID(HIWORD(GetKeyboardLayout(0))))
		{
		case LANG_FRENCH:
			BlasterCharacter->BlastCharacterMappingContext->UnmapAll();
			BlasterCharacter->BlastCharacterMappingContext->Mappings = BlasterCharacter->BlastCharacterMappingContextAZERTY->GetMappings();
			break;
		case LANG_ENGLISH:
			BlasterCharacter->BlastCharacterMappingContext->UnmapAll();
			BlasterCharacter->BlastCharacterMappingContext->Mappings = BlasterCharacter->BlastCharacterMappingContextQWERTY->GetMappings();
			break;
		default:
			BlasterCharacter->BlastCharacterMappingContext->UnmapAll();
			BlasterCharacter->BlastCharacterMappingContext->Mappings = BlasterCharacter->BlastCharacterMappingContextQWERTY->GetMappings();
			break;
		}
		//Set slider to default sensitivity
		if (MouseSensitivitySlider)
		{
			MouseSensitivitySlider->SetValue(BlasterCharacter->GetDefaultSensitivity());
		}
		if (AimSensitivitySlider)
		{
			AimSensitivitySlider->SetValue(BlasterCharacter->GetDefaultAimSensitivity());
		}

	}
	if (SettingBox)
	{
		SettingBox->ClearChildren();
	}
	for (const FEnhancedActionKeyMapping Mapping : BlasterCharacter->BlastCharacterMappingContext->GetMappings())
	{
		UKeyMappingButton* KeyMappingButton = CreateWidget<UKeyMappingButton>(this, KeyMappingButtonClass);
		if (KeyMappingButton)
		{
			UPlayerMappableKeySettings* PlayerMappableKeySettings = Mapping.GetPlayerMappableKeySettings();
			if (PlayerMappableKeySettings)
			{
				FText ActionName = Mapping.GetPlayerMappableKeySettings()->DisplayName;
				KeyMappingButton->InputKey1->SetSelectedKey(Mapping.Key);
				KeyMappingButton->KeyLabelText->SetText(ActionName);
				KeyMappingButton->AddToViewport();
				SettingBox->AddChild(KeyMappingButton);
				KeyMappingButton->KeyIndex = index;
			}

		}
		index++;
	}



	//Save the mapping context
	USaveInputMapping* SaveGameInstance = Cast<USaveInputMapping>(UGameplayStatics::CreateSaveGameObject(USaveInputMapping::StaticClass()));
	SaveGameInstance->EnhancedActionMappings = BlasterCharacter->BlastCharacterMappingContext->GetMappings();
	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("BlasterInputMapping"), 0);
}

void UMenuInGame::ContactMeButtonClicked()
{
	ShowMenuPanel(ESlateVisibility::Hidden);
	ShowContactMePanel(ESlateVisibility::Visible);

}

void UMenuInGame::CreditsButtonClicked()
{
	ShowMenuPanel(ESlateVisibility::Hidden);
	ShowCreditsPanel(ESlateVisibility::Visible);
}

void UMenuInGame::OnMouseSensitivityValueChanged(float Value)
{
	if (BlasterCharacter)
	{
		BlasterCharacter->SetSensitivity(Value);
		if (ValueMouseSensitivityText)
		{
			ValueMouseSensitivityText->SetText(FText::AsNumber(Value));
		}
		//Save the sensitivity
		USaveSensitivity* SaveGameInstance = Cast<USaveSensitivity>(UGameplayStatics::CreateSaveGameObject(USaveSensitivity::StaticClass()));
		SaveGameInstance->MouseSensitivity = Value;
		SaveGameInstance->AimSensitivity = BlasterCharacter->GetAimSensitivity();
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("BlasterSensitivity"), 0);
	}
}

void UMenuInGame::OnAimSensitivityValueChanged(float Value)
{
	if (BlasterCharacter)
	{
		BlasterCharacter->SetAimSensitivity(Value);
		if (ValueAimSensitivityText)
		{
			ValueAimSensitivityText->SetText(FText::AsNumber(Value));
		}
		//Save the sensitivity
		USaveSensitivity* SaveGameInstance = Cast<USaveSensitivity>(UGameplayStatics::CreateSaveGameObject(USaveSensitivity::StaticClass()));
		SaveGameInstance->AimSensitivity = Value;
		SaveGameInstance->MouseSensitivity = BlasterCharacter->GetSensitivity();
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("BlasterSensitivity"), 0);
	}
}

void UMenuInGame::OnQualityComboBoxValueChanged(FString Value, ESelectInfo::Type SelectionType)
{
	int32 Quality = 0;
	if (Value == "Low")
	{
		Quality = 0;
	}
	else if (Value == "Medium")
	{
		Quality = 1;
	}
	else if (Value == "High")
	{
		Quality = 2;
	}
	else if (Value == "Epic")
	{
		Quality = 3;
	}
	//Set all quality to the same value
	UGameUserSettings::GetGameUserSettings()->ScalabilityQuality.SetFromSingleQualityLevel(Quality);
	UGameUserSettings::GetGameUserSettings()->ApplySettings(true);

}

void UMenuInGame::OnDisplayModeComboBoxValueChanged(FString Value, ESelectInfo::Type SelectionType)
{
	//Get game user settings
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		EWindowMode::Type DisplayMode;
		if (Value == "Fullscreen")
		{
			DisplayMode = EWindowMode::Fullscreen;
			GameUserSettings->SetScreenResolution(GameUserSettings->GetDesktopResolution());
			DisplayResolutionComboBox->SetIsEnabled(true);

		}
		else if (Value == "Windowed Fullscreen")
		{
			DisplayMode = EWindowMode::WindowedFullscreen;
			//Block to change the resolution DisplayResolutionComboBox
			DisplayResolutionComboBox->SetIsEnabled(false);
		}
		else if (Value == "Windowed")
		{
			DisplayMode = EWindowMode::Windowed;
			GameUserSettings->SetScreenResolution(GameUserSettings->GetDesktopResolution() / 2);
			DisplayResolutionComboBox->SetIsEnabled(true);
		}
		GameUserSettings->SetFullscreenMode(DisplayMode);
		GameUserSettings->ApplyResolutionSettings(false);
		UpdateDisplayResolutionComboBox();
	}
}

void UMenuInGame::OnDisplayResolutionComboBoxValueChanged(FString Value, ESelectInfo::Type SelectionType)
{
	//Get game user settings
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	if (GameUserSettings)
	{
		FString XString;
		FString YString;
		Value.Split(" x ", &XString, &YString);
		int32 X = FCString::Atoi(*XString);
		int32 Y = FCString::Atoi(*YString);
		FIntPoint Resolution = FIntPoint(X, Y);
		GameUserSettings->SetScreenResolution(Resolution);
		GameUserSettings->ApplyResolutionSettings(false);
	}
}

void UMenuInGame::ClearSettingBox()
{
	if (SettingBox)
	{
		SettingBox->ClearChildren();
	}
	
}

void UMenuInGame::ShowMenuPanel(ESlateVisibility bShow)
{
	if (MenuPanel)
	{
		MenuPanel->SetVisibility(bShow);
	}
	
}

void UMenuInGame::ShowSettingPanel(ESlateVisibility bShow)
{
	if (SettingPanel)
	{
		SettingPanel->SetVisibility(bShow);
	}
	if (bShow == ESlateVisibility::Hidden)
	{
		ClearSettingBox();
	}
}

void UMenuInGame::ShowContactMePanel(ESlateVisibility bShow)
{
	if (ContactMePanel)
	{
		ContactMePanel->SetVisibility(bShow);
	}
}

void UMenuInGame::ShowCreditsPanel(ESlateVisibility bShow)
{
	if (CreditsPanel)
	{
		CreditsPanel->SetVisibility(bShow);
	}

}

void UMenuInGame::ShowGraphicSettingPanel(ESlateVisibility bShow)
{
	if (GraphicSettingPanel)
	{
		GraphicSettingPanel->SetVisibility(bShow);
	}
}


