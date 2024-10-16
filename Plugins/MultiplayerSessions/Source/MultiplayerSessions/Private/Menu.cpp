// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "GameFramework/GameUserSettings.h"


void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch,FString LobbyPath, FString Map)
{
	MaxNumPlayers = NumberOfPublicConnections;
	GameMode = TypeOfMatch;
	MapName = Map;
	PathToLobbyMap = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;
	FInputModeUIOnly InputModeData;
	InputModeData.SetWidgetToFocus(TakeWidget());
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = true;

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!ensure(GameInstance != nullptr)) return;
	MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);

	}
	SteamAPI_RestartAppIfNecessary(atoi(APP_ID));

	FString CurrentGameLanguage = FString(SteamApps()->GetCurrentGameLanguage());

	//List of languages de el en es fr it ja pt ru uk vi zh-Hant
	if (CurrentGameLanguage == "japanese") FInternationalization::Get().SetCurrentCulture(TEXT("ja"));
	if (CurrentGameLanguage == "french") FInternationalization::Get().SetCurrentCulture(TEXT("fr"));
	if (CurrentGameLanguage == "german") FInternationalization::Get().SetCurrentCulture(TEXT("de"));
	if (CurrentGameLanguage == "spanish") FInternationalization::Get().SetCurrentCulture(TEXT("es"));
	if (CurrentGameLanguage == "italian") FInternationalization::Get().SetCurrentCulture(TEXT("it"));
	if (CurrentGameLanguage == "greek") FInternationalization::Get().SetCurrentCulture(TEXT("el"));
	if (CurrentGameLanguage == "portuguese") FInternationalization::Get().SetCurrentCulture(TEXT("pt"));
	if (CurrentGameLanguage == "russian") FInternationalization::Get().SetCurrentCulture(TEXT("ru"));
	if (CurrentGameLanguage == "ukrainian") FInternationalization::Get().SetCurrentCulture(TEXT("uk"));
	if (CurrentGameLanguage == "vietnamese") FInternationalization::Get().SetCurrentCulture(TEXT("vi"));
	if (CurrentGameLanguage == "tchinese") FInternationalization::Get().SetCurrentCulture(TEXT("zh-Hant"));
	if (CurrentGameLanguage == "english") FInternationalization::Get().SetCurrentCulture(TEXT("en"));
	
	
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}
	if (CreateButton)
	{
		CreateButton->OnClicked.AddDynamic(this, &ThisClass::CreateButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (Settings)
	{
		Settings->RunHardwareBenchmark();
		float CPUBenchmark = Settings->GetLastCPUBenchmarkResult();
		float GPUBenchmark = Settings->GetLastGPUBenchmarkResult();
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("CPUBenchmark : %f"), CPUBenchmark));
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("GPUBenchmark : %f"), GPUBenchmark));
		// D�terminez le niveau de qualit� en fonction des r�sultats du benchmark
		int32 Quality;
		if (CPUBenchmark > 250.0f && GPUBenchmark > 800.0f)
		{
			Quality = 3; // Epic
		}
		else if (CPUBenchmark > 100.0f && GPUBenchmark > 290.0f)
		{
			Quality = 2; // High
		}
		else if (CPUBenchmark > 50.0f && GPUBenchmark > 150.0f)
		{
			Quality = 1; // Medium
		}
		else
		{
			Quality = 0; // Low
		}

		// Appliquez le niveau de qualit�
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Quality : %d"), Quality));
		Settings->ScalabilityQuality.SetFromSingleQualityLevel(Quality);
		Settings->ApplySettings(false);
		
	}
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Menu Initialized"));
	UCommonActivatableWidget::ActivateWidget();
	UCommonActivatableWidget::GetDesiredFocusTarget();
	return true;
}

void UMenu::NativeDestruct()
{
	Super::NativeDestruct();
	MenuTeardown();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Session Created"));
		}
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(PathToLobbyMap);
		}
		
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Session Creation Failed"));
		}		
		CreateButton->SetIsEnabled(true);
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (MultiplayerSessionsSubsystem == nullptr)
	{
		return;
	}
	if (MainMenuPanel && JoinMenuPanel)
	{
		ShowJoinMenu(ESlateVisibility::Visible);
		ShowMainMenu(ESlateVisibility::Hidden);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Menu Panels not found"));
	}
	InfoJoinText->SetText(FText::FromString(""));
	ClearGamesBox();
	for (auto Result : SessionResults)
	{

		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("GameMode"), SettingsValue);

		FString SettingsValueMap;
		Result.Session.SessionSettings.Get(FName("MapName"), SettingsValueMap);

		//FString SettingsValueNumPlayers;
		//Result.Session.SessionSettings.Get(FName("NumPlayers"), SettingsValueNumPlayers);
		
		
		int32 SettingsValueMaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
		//GetNumLobbyMembers
		int32 SettingsValueNumPlayersInt = Result.Session.SessionSettings.NumPublicConnections - Result.Session.NumOpenPublicConnections;
		//int32 SettingsValueNumPlayersInt = Result.Session.SessionSettings.Get(FName("NumPlayers"), SettingsValueNumPlayersInt);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("NumPlayers: %d"), SettingsValueNumPlayersInt));

		//Get MultiplayerSessionsSubsystem from the Result
		//UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem = Result.Session.SessionSettings.Get(FName("MultiplayerSessionsSubsystem"), MultiplayerSessionsSubsystem);

		FString SettingsValueNumPlayersText = FString::Printf(TEXT("%d/%d"), SettingsValueNumPlayersInt, SettingsValueMaxPlayers);


		//Create UButtonJoinGame
		UButtonJoinGame* ButtonJoinGame = CreateWidget<UButtonJoinGame>(this, ButtonJoinGameClass);


		//add button to GamesBox
		if (GamesBox)
		{
			ButtonJoinGame->AddToViewport();
			GamesBox->AddChild(ButtonJoinGame);
			ButtonJoinGame->HostNameText->SetText(FText::FromString(Result.Session.OwningUserName));
			ButtonJoinGame->GameModeNameText->SetText(FText::FromString(SettingsValue));
			ButtonJoinGame->MapNameText->SetText(FText::FromString(SettingsValueMap));
			ButtonJoinGame->NumPlayersText->SetText(FText::FromString(SettingsValueNumPlayersText));
			ButtonJoinGame->SearchResult = Result;
			ButtonJoinGame->MultiplayerSubsystem = MultiplayerSessionsSubsystem;

		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GamesBox not found"));
		}
		if (SettingsValue == GameMode)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Session Found"));
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, SettingsValue);
			}
			//MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}
	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Session Find Failed"));
			
		}
		if (InfoJoinText)
		{
			
			//add button to GamesBox
			//for (size_t i = 0; i < 30; i++)
			//{
			//	UButtonJoinGame* ButtonJoinGame = CreateWidget<UButtonJoinGame>(this, ButtonJoinGameClass);
			//	if (GamesBox)
			//	{

			//		ButtonJoinGame->AddToViewport();
			//		GamesBox->AddChild(ButtonJoinGame);

			//	}
			//	else
			//	{
			//		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GamesBox not found"));
			//	}
			//}

			InfoJoinText->SetText(FText::FromString("No Sessions Found"));
		}
		
		
	}
	JoinButton->SetIsEnabled(true);
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Joining Session"));
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString Address;
			if (SessionInterface->GetResolvedConnectString(NAME_GameSession, Address))
			{
				APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
				if (PlayerController)
				{
					PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
				}
			}
		}
	}
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Session Join Failed"));
			MultiplayerSessionsSubsystem->DestroySession();

		}
		JoinButton->SetIsEnabled(true);
	}
}



void UMenu::OnStartSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Session Started"));
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Session Start Failed"));
		}
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Session Destroyed"));
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Session Destroy Failed"));
		}
	}
}



void UMenu::ShowJoinMenu(ESlateVisibility bShow)
{
	if (JoinMenuPanel)
	{
		JoinMenuPanel->SetVisibility(bShow);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Join Menu Panel not found"));
	}

}

void UMenu::ShowMainMenu(ESlateVisibility bShow)
{
	if (MainMenuPanel)
	{
		MainMenuPanel->SetVisibility(bShow);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Main Menu Panel not found"));
	}
	if (JoinButton->bIsEnabled == false)
	{
			JoinButton->SetIsEnabled(true);
	}

}

void UMenu::ShowCreateMenu(ESlateVisibility bShow)
{
	if (CreateMenuPanel)
	{
		CreateMenuPanel->SetVisibility(bShow);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Create Menu Panel not found"));
	}
}

void UMenu::ClearGamesBox()
{
	if (GamesBox)
	{
		GamesBox->ClearChildren();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GamesBox not found"));
	}

}

void UMenu::CreateButtonClicked()
{
	CreateButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(MaxNumPlayers, GameMode, MapName);
	}
}

void UMenu::JoinButtonClicked()
{
	MultiplayerSessionsSubsystem->DestroySession();
	JoinButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void UMenu::MenuTeardown()
{
	RemoveFromParent();
	SetVisibility(ESlateVisibility::Hidden);
	bIsFocusable = false;

	UWorld* World = GetWorld();
	if (!ensure(World != nullptr)) return;
	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!ensure(PlayerController != nullptr)) return;
	FInputModeGameOnly InputModeData;
	PlayerController->SetInputMode(InputModeData);
	PlayerController->bShowMouseCursor = false;
}


