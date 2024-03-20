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
#include "Components/ScaleBox.h"
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
#include "GenericPlatform/GenericApplication.h"
#include <SetupAPI.h>



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
	if (DisplayMonitorComboBox && !DisplayMonitorComboBox->OnSelectionChanged.IsBound())
	{
		DisplayMonitorComboBox->OnSelectionChanged.AddDynamic(this, &UMenuInGame::OnDisplayMonitorComboBoxValueChanged);
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
		FString DisplayModeString;
		switch (CurrentDisplayMode)
		{
		//case EWindowMode::Fullscreen:
		//	DisplayModeString = TEXT("Fullscreen");
		//	break;
		case EWindowMode::WindowedFullscreen:
			DisplayModeString = TEXT("Windowed Fullscreen");
			DisplayResolutionComboBox->SetIsEnabled(false);
			break;
		case EWindowMode::Windowed:
			DisplayModeString = TEXT("Windowed");
			DisplayResolutionComboBox->SetIsEnabled(true);
			break;
		default:
			DisplayModeString = TEXT("Unknown");
			break;
		}

		// Affichage du mode d'affichage actuel
		switch (CurrentDisplayMode)
		{
			//case EWindowMode::Fullscreen:
			//DisplayModeComboBox->SetSelectedIndex(0);
			//break;
			case EWindowMode::WindowedFullscreen:
				DisplayModeComboBox->SetSelectedIndex(0);
				break;
			case EWindowMode::Windowed:
				DisplayModeComboBox->SetSelectedIndex(1);
			break;

		}
	}
	//Set Display Resolution
	UpdateDisplayResolutionComboBox();
	int32 i = 0;
	//Set Display Monitor
	if (DisplayMonitorComboBox)
	{
		
		//Get Display Monitor available
		TArray<FString> MonitorNames;
		FDisplayMetrics DisplayMetrics;
		FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);
		DisplayMonitorComboBox->ClearOptions();
		//FVector2D WindowPosition = FSlateApplication::Get().GetActiveTopLevelWindow()->GetPositionInScreen();
		FVector2D WindowPosition = GEngine->GameViewport->GetWindow()->GetPositionInScreen();
		for (const FMonitorInfo& MonitorInfo : DisplayMetrics.MonitorInfo)
		{
			i++;
			DisplayMonitorComboBox->AddOption(MonitorInfo.Name);
			/*if (MonitorInfo.bIsPrimary)
			{
				DisplayMonitorComboBox->SetSelectedOption(MonitorInfo.Name);
			}*/
			
			//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red, FString::Printf(TEXT("Window Position: %f x %f"), WindowPosition.X, WindowPosition.Y));
			//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red, FString::Printf(TEXT("I = %d"), i));
			//FVector2D WindowSize = GEngine->GameViewport->GetWindow()->GetClientSizeInScreen();
			//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red, FString::Printf(TEXT("Window Size: %f x %f"), WindowSize.X, WindowSize.Y));
			
			bool isWindowOnMonitor = IsWindowOnMonitor(WindowPosition, MonitorInfo);			
			if (IsWindowOnMonitor(WindowPosition, MonitorInfo))
			{
				//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red, FString::Printf(TEXT("%d Monitor Name: %s"), i, *MonitorInfo.Name));
				DisplayMonitorComboBox->SetSelectedOption(MonitorInfo.Name);
			}
		}
		//Get real name of the monitor
		//GUID MonitorClassGuid = { 0x4d36e96e, 0xe325, 0x11ce, {0xbf, 0xc1, 0x08, 0x00, 0x2b, 0xe1, 0x03, 0x18} };
		//HDEVINFO hDevInfo = SetupDiGetClassDevs(&MonitorClassGuid, NULL, NULL, DIGCF_PRESENT);
		////int32 MonitorCount = -1;
		//if (hDevInfo != INVALID_HANDLE_VALUE)
		//{
		//	
		//	SP_DEVINFO_DATA DeviceInfoData;
		//	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		//	for (DWORD i = 0;SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
		//	{
		//		//MonitorCount++;
		//		DWORD DataT;
		//		LPTSTR buffer = NULL;
		//		DWORD buffersize = 0;

		//		while (!SetupDiGetDeviceRegistryProperty(
		//			hDevInfo,
		//			&DeviceInfoData,
		//			SPDRP_FRIENDLYNAME,
		//			&DataT,
		//			(PBYTE)buffer,
		//			buffersize,
		//			&buffersize))
		//		{
		//			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
		//			{
		//				if (buffer) LocalFree(buffer);
		//				buffer = (LPTSTR)LocalAlloc(LPTR, buffersize * 2);
		//			}
		//			else
		//			{
		//				break;
		//			}
		//		}
		//		if (buffer)
		//		{
		//			FRegexPattern Pattern(TEXT("\\((.*)\\)"));
		//			FRegexMatcher Matcher(Pattern, buffer);
		//			if (Matcher.FindNext())
		//			{
		//				FString MonitorName = Matcher.GetCaptureGroup(1);
		//				DisplayMonitorComboBox->AddOption(MonitorName);
		//				UE_LOG(LogTemp, Warning, TEXT("Monitor Name: %s"), *MonitorName);
		//			}
		//			else
		//			{
		//				DisplayMonitorComboBox->AddOption(buffer);
		//			}
		//			//DisplayList.Add(DisplayMetrics.MonitorInfo[MonitorCount].Name);
		//			//DisplayMonitorComboBox->AddOption(buffer);

		//			GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Green, FString::Printf(TEXT("Monitor Name: %s"), *FString(buffer)));
		//			GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red, FString::Printf(TEXT("Monitor Name: %s"), *DisplayMetrics.MonitorInfo[i].Name));
		//			//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red, FString::Printf(TEXT("i = %d"), MonitorCount));
		//			if (DisplayMetrics.MonitorInfo[i].bIsPrimary)
		//			{
		//				UE_LOG(LogTemp, Warning, TEXT("Primary Monitor Name: %s"), *DisplayMetrics.MonitorInfo[i].Name);
		//				DisplayMonitorComboBox->SetSelectedIndex(i);
		//			}
		//			LocalFree(buffer);
		//		}
		//	}
		//	SetupDiDestroyDeviceInfoList(hDevInfo);

		//}
	}
	


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
	if (SelectionType == ESelectInfo::OnMouseClick)
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

}

void UMenuInGame::OnDisplayModeComboBoxValueChanged(FString Value, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::OnMouseClick)
	{
		//Get game user settings
		UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
		if (GameUserSettings)
		{
			EWindowMode::Type DisplayMode;
			//if (Value == "Fullscreen")
			//{
			//	DisplayMode = EWindowMode::Fullscreen;
			//	//GameUserSettings->SetFullscreenMode(DisplayMode);
			//	//Get resolution of the screen
			//	FDisplayMetrics DisplayMetrics;
			//	FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);
			//	//set the resolution to the screen resolution
			//	//FIntPoint Resolution = FIntPoint(DisplayMetrics.PrimaryDisplayWidth, DisplayMetrics.PrimaryDisplayHeight);
			//	////GameUserSettings->SetScreenResolution(Resolution);
			//	//GEngine->GameViewport->GetWindow()->Resize(Resolution);
			//	//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red, FString::Printf(TEXT("Resolution: %d x %d"), Resolution.X, Resolution.Y));

			//	//Get screen where the window is
			//	FVector2D WindowPosition = FSlateApplication::Get().GetActiveTopLevelWindow()->GetPositionInScreen();
			//	for (const FMonitorInfo& MonitorInfo : DisplayMetrics.MonitorInfo)
			//	{
			//		if (IsWindowOnMonitor(WindowPosition, MonitorInfo))
			//		{
			//			FString MonitorName = MonitorInfo.Name;
			//			//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red, FString::Printf(TEXT("Monitor Name: %s"), *MonitorName));
			//			//set the resolution to the screen resolution
			//			FIntPoint Resolution = FIntPoint(MonitorInfo.NativeWidth, MonitorInfo.NativeHeight);

			//			//GEngine->GameViewport->GetWindow()->Resize(Resolution);
			//			//Monitor name
			//			//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red, FString::Printf(TEXT("Monitor Name: %s"), *MonitorName));
			//			//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red, FString::Printf(TEXT("Resolution: %d x %d"), Resolution.X, Resolution.Y));
			//			GameUserSettings->SetScreenResolution(Resolution);
			//			//GEngine->GameViewport->GetWindow()->Resize(Resolution);
			//			

			//			//Change viewport size to the new resolution



			//			//SetDrawSize(HUDSize);
			//			
			//			//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red, FString::Printf(TEXT("New Resolution Size: %d x %d"), Resolution.X, Resolution.Y));
			//			//FVector2D HUDSize = GEngine->GameViewport->Viewport->GetSizeXY();
			//			//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red, FString::Printf(TEXT("HUD Size: %f x %f"), HUDSize.X, HUDSize.Y));

			//			//resize the scalebox to the new resolution
			//			//ScaleBox->SetUserSpecifiedScale(Resolution.X / 1920.f);
			//			//scale box size
			//			FVector2D ScaleBoxSize = ScaleBox->GetDesiredSize();
			//			GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red, FString::Printf(TEXT("Scale Box Size: %f x %f"), ScaleBoxSize.X, ScaleBoxSize.Y));
			//			//GetWorld()->GetGameViewport()->HandleToggleFullscreenCommand();
			//			GameUserSettings->SetFullscreenMode(DisplayMode);
			//			GameUserSettings->ApplySettings(false);
			//			GameUserSettings->ApplyResolutionSettings(false);


			//			break;
			//		}
			//	}



			//	DisplayResolutionComboBox->SetIsEnabled(true);

			//}
			if (Value == "Windowed Fullscreen")
			{
				DisplayMode = EWindowMode::WindowedFullscreen;

				//Block to change the resolution DisplayResolutionComboBox
				DisplayResolutionComboBox->SetIsEnabled(false);
				GameUserSettings->SetFullscreenMode(DisplayMode);
				GameUserSettings->ApplyResolutionSettings(false);
				GameUserSettings->ApplySettings(false);
			}
			else if (Value == "Windowed")
			{
				DisplayMode = EWindowMode::Windowed;
				GameUserSettings->SetScreenResolution(GameUserSettings->GetDesktopResolution() / 2);
				DisplayResolutionComboBox->SetIsEnabled(true);
				GameUserSettings->SetFullscreenMode(DisplayMode);
				GameUserSettings->ApplyResolutionSettings(false);
				GameUserSettings->ApplySettings(false);
			}

			UpdateDisplayResolutionComboBox();
		}
	}
}
bool UMenuInGame::IsWindowOnMonitor(FVector2D WindowPosition, FMonitorInfo MonitorInfo)
{
	// Assurez-vous que le point est � l'int�rieur de la zone de travail du moniteur, inclusivement
	return WindowPosition.X >= MonitorInfo.WorkArea.Left && WindowPosition.X < MonitorInfo.WorkArea.Right &&
		WindowPosition.Y >= MonitorInfo.WorkArea.Top && WindowPosition.Y < MonitorInfo.WorkArea.Bottom;
}



void UMenuInGame::OnDisplayResolutionComboBoxValueChanged(FString Value, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::OnMouseClick)
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
}

void UMenuInGame::OnDisplayMonitorComboBoxValueChanged(FString Value, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::OnMouseClick)
	{
		FDisplayMetrics DisplayMetrics;
		FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);

		// Check if the monitor index is within the valid range
		if (DisplayMetrics.MonitorInfo.IsValidIndex(0))
		{
			// Get the monitor index
			int32 MonitorIndex = 0;
			for (int32 Index = 0; Index < DisplayMetrics.MonitorInfo.Num(); Index++)
			{
			   if (DisplayMetrics.MonitorInfo[Index].Name == Value)
			   {
				   MonitorIndex = Index;
					break;
			   }
			}

			// Get the window position
			FVector2D WindowPosition = FVector2D(DisplayMetrics.MonitorInfo[MonitorIndex].WorkArea.Left, DisplayMetrics.MonitorInfo[MonitorIndex].WorkArea.Top);

			// Move the window to the monitor
			GEngine->GameViewport->GetWindow()->MoveWindowTo(WindowPosition);
			// Set to the current Display Mode
			EWindowMode::Type DisplayMode;

			//Switch DisplayModeComboBox
			switch (DisplayModeComboBox->GetSelectedIndex())
			{
			case 0:
			{
				DisplayMode = EWindowMode::WindowedFullscreen;
				UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
				if (UserSettings)
				{
					// D�finir la r�solution souhait�e et le mode plein �cran fen�tr�
					//UserSettings->SetScreenResolution(FIntPoint(1707, 1067));
					UserSettings->SetFullscreenMode(EWindowMode::Windowed);

					// Appliquer les changements
					UserSettings->ApplySettings(false);

					UserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);

					// Appliquer les changements
					UserSettings->ApplySettings(false);
				}
				break;
			}
			case 1:
				DisplayMode = EWindowMode::Windowed;
				break;
			}

		// Get the window size
	   // FVector2D WindowSize = FVector2D(DisplayMetrics.MonitorInfo[MonitorIndex].WorkArea.Right - DisplayMetrics.MonitorInfo[MonitorIndex].WorkArea.Left, DisplayMetrics.MonitorInfo[MonitorIndex].WorkArea.Bottom - DisplayMetrics.MonitorInfo[MonitorIndex].WorkArea.Top);
	


		FVector2D WindowSize = FVector2D(DisplayMetrics.MonitorInfo[MonitorIndex].NativeWidth, DisplayMetrics.MonitorInfo[MonitorIndex].NativeHeight);
		// Resize the window
		//GEngine->GameViewport->GetWindow()->Resize(WindowSize);

		//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red, FString::Printf(TEXT("Window Size: %f x %f"), WindowSize.X, WindowSize.Y));

		//UGameUserSettings::GetGameUserSettings()->SetFullscreenMode(DisplayMode);


		//GEngine->AddOnScreenDebugMessage(-1, 25.f, FColor::Red, FString::Printf(TEXT("Display Mode: %d"), DisplayMode));


		//// Apply the settings
		//UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
		//UGameUserSettings::GetGameUserSettings()->ApplyResolutionSettings(false);

		UpdateDisplayResolutionComboBox();

		}
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


