// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/HUD/CharacterOverlay.h"
#include "Blaster/HUD/AnnouncementWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Components/MultiLineEditableTextBox.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/GameMode/BlasterGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Blaster/BlasterComponents/CombatComponent.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include <Blaster/GameState/BlasterGameState.h>
#include "Components/Image.h"
#include "GameFramework/PlayerState.h"
#include "Blaster/HUD/MenuInGame.h"
#include "Blaster/BlasterTypes/Annoucement.h"
#include "Components/Border.h"
#include <Blaster/HUD/ChatBox.h>
#include "Components/WrapBox.h"
#include "Components/EditableTextBox.h"
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/HUD/BlasterHUD.h"
#include <Blueprint/WidgetLayoutLibrary.h>
#include "DLSSLibrary.h"







void ABlasterPlayerController::BrodcastKillFeed(APlayerState* Killer, APlayerState* Victim,  EWeaponType WeaponTypeUsed)
{
	ClientKillFeed(Killer, Victim, WeaponTypeUsed);
}

void ABlasterPlayerController::ClientKillFeed_Implementation(APlayerState* Killer, APlayerState* Victim, EWeaponType WeaponTypeUsed)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (Killer && Victim && BlasterHUD)
	{
		FText KillerName = Killer ? FText::FromString(Killer->GetPlayerName()) : FText::FromString("Unknown");
		FText VictimName = Victim ? FText::FromString(Victim->GetPlayerName()) : FText::FromString("Unknown");
		APlayerState* Self = GetPlayerState<APlayerState>();
		FText SelfName = Self ? FText::FromString(Self->GetPlayerName()) : FText::FromString("Unknown");

		BlasterHUD->CharacterOverlay->SetKillFeedText(KillerName, VictimName, SelfName, WeaponTypeUsed);

	}
}



void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	ServerCheckMatchState();
	if (ChatWidgetClass)
	{
		APlayerController* LocalPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		ChatWidget = CreateWidget<UChatBox>(LocalPlayerController, ChatWidgetClass);

		if (ChatWidget)
		{
			ChatWidget->AddToViewport();
			ChatWidget->ChatInputText->OnTextCommitted.AddDynamic(this, &ABlasterPlayerController::OnChatMessageSent);
			ChatWidget->SetVisibility(ESlateVisibility::Hidden);			
		}
	}

	
}

void ABlasterPlayerController::SetHUDMatchTime()
{
	float TimeLeft = 0.f;

	if (MatchState == MatchState::WaitingToStart) TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime - lastTime;
	else if (MatchState == MatchState::InProgress) {
		TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
		lastTime = TimeLeft;
	}
	else if (MatchState == MatchState::Cooldown) TimeLeft = CooldownTime + WarmupTime + MatchTime - GetServerTime() + LevelStartingTime - lastTime;

	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	if (HasAuthority())
	{
		if (BlasterGameMode == nullptr)
		{
			BlasterGameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));

			if (BlasterGameMode != nullptr)
			{
				LevelStartingTime = BlasterGameMode->LevelStartingTime;
			}
		}
		BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
		if (BlasterGameMode)
		{
			SecondsLeft = FMath::CeilToInt(BlasterGameMode->GetCountdownTime() + LevelStartingTime);
		}
	}
	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDWarmupCountdown(TimeLeft);

		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
		
	}
	CountdownInt = SecondsLeft;

}




void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->HealthBar && BlasterHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		const float HealthPercentage = Health / MaxHealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercentage);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeHealth = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}

}

void ABlasterPlayerController::SetHUDShield(float Shield, float MaxShield)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->ShieldBar && BlasterHUD->CharacterOverlay->ShieldText;
	if (bHUDValid)
	{
		const float ShieldPercentage = Shield / MaxShield;
		BlasterHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercentage);
		FString ShieldText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Shield), FMath::CeilToInt(MaxShield));
		BlasterHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldText));
	}
	else
	{
		bInitializeShield = true;
		HUDShield = Shield;
		HUDMaxShield = MaxShield;
	}
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->ScoreAmountText;
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		BlasterHUD->CharacterOverlay->ScoreAmountText->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeScore = true;
		HUDScore = Score;
	}
}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->DefeatsAmountText;
	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		BlasterHUD->CharacterOverlay->DefeatsAmountText->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		bInitializeDefeats = true;
		HUDDefeats = Defeats;
	}
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->WeaponAmmoAmountText;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->CharacterOverlay->WeaponAmmoAmountText->SetText(FText::FromString(AmmoText));
	}
	else
	{
		bInitializeWeaponAmmo = true;
		HUDWeaponAmmo = Ammo; 
	}
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->CarriedAmmoAmountText;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->CharacterOverlay->CarriedAmmoAmountText->SetText(FText::FromString(AmmoText));
	}
	else
	{
		bInitializeCarriedAmmo = true;
		HUDCarriedAmmo = Ammo;
	}
}

void ABlasterPlayerController::SetHUDKillFieldInfo(const FString& KillerName, const FString& VictimName)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->DefeatsAmountText;
	FString KillFieldText;
	if (bHUDValid)
	{
		if (KillerName == VictimName)
		{
			//KillFieldText = FString::Printf(TEXT("%s killed himself"), *KillerName);
			//KillFieldText = FString::Printf(SuicideText, *KillerName);
			//KillFieldText = FText::Format(SuicideText, *KillerName).ToString();
			KillFieldText = FText::Format(SuicideText, FText::FromString(KillerName)).ToString();

		}
		else
		{
			//KillFieldText = FString::Printf(TEXT("%s killed %s"), *KillerName, *VictimName);
			//KillFieldText = FString::Printf(KillWithText, *KillerName, *VictimName);
			//KillFieldText = FText::Format(KillWithText, *KillerName, *VictimName).ToString();
			KillFieldText = FText::Format(KillWithText, FText::FromString(KillerName), FText::FromString(VictimName)).ToString();
		}
	}
}

void ABlasterPlayerController::SetHUDKillFieldPlayerInfo(const FString& PlayerName, bool bIsDead)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->DefeatsAmountText;
	FString KillFieldText;
	if (bHUDValid)
	{
		if (PlayerName == "")
		{
			KillFieldText = "";
		}
		else
		{
			if (bIsDead)
			{
				//KillFieldText = FString::Printf(TEXT("Killed by %s"), *PlayerName);
				//const FText KillByText = NSLOCTEXT("BlasterPlayerController", "KillByText", "Killed by"); 
				//KillFieldText = KillByText + $s, *PlayerName
				//KillFieldText = FString::Printf(*KillByText.ToString(), *PlayerName);
				//KillFieldText = FText::Format(KillByText, *PlayerName).ToString();
				KillFieldText = FText::Format(KillByText, FText::FromString(PlayerName)).ToString();


			}
			else
			{
				//KillFieldText = FString::Printf(TEXT("Killed %s"), *PlayerName);
				//KillFieldText = FString::Printf(KillText, *PlayerName);
				//KillFieldText = FString::Printf(*KillText.ToString(), *PlayerName);
				//KillFieldText = FText::Format(KillText, *PlayerName).ToString();
				KillFieldText = FText::Format(KillText, FText::FromString(PlayerName)).ToString();
			}
		}
			BlasterHUD->CharacterOverlay->KillFieldPlayerText->SetText(FText::FromString(KillFieldText));


	}
}

void ABlasterPlayerController::SetHUDMatchCountdown(float Countdown)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->MatchCountdownText;
	if (bHUDValid)
	{
		if (Countdown < 0.f)
		{
			BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}
		int32 Minutes = FMath::FloorToInt(Countdown / 60);
		int32 Seconds = Countdown - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDWarmupCountdown(float Countdown)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->AnnouncementOverlay && BlasterHUD->AnnouncementOverlay->WarmupTimeText;
	if (bHUDValid)
	{
		if (Countdown < 0.f)
		{
			BlasterHUD->AnnouncementOverlay->WarmupTimeText->SetText(FText());
			return;
		}

		int32 Minutes = FMath::FloorToInt(Countdown / 60);
		int32 Seconds = Countdown - Minutes * 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->AnnouncementOverlay->WarmupTimeText->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerController::SetHUDGrenadeAmount(int32 Amount)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->GrenadeAmountText;
	if (bHUDValid)
	{
		FString GrenadesText = FString::Printf(TEXT("%d"), Amount);
		BlasterHUD->CharacterOverlay->GrenadeAmountText->SetText(FText::FromString(GrenadesText));
	}
	else
	{
		bInitializeGrenadeAmount = true;
		HUDGrenadeAmount = Amount;
	}
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if (BlasterCharacter)
	{
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
	}
}

void ABlasterPlayerController::Tick(float DeltaTimes)
{
	Super::Tick(DeltaTimes);

	SetHUDMatchTime();
	CheckTimeSync(DeltaTimes);
	PollInit();
	CheckPing(DeltaTimes);

}
void ABlasterPlayerController::CheckPing(float DeltaTime)
{
	if (HasAuthority()) return;
	HighPingRunningTime += DeltaTime;

	if (HighPingRunningTime > CheckPingFrequency)
	{
		PlayerState = PlayerState == nullptr ? GetPlayerState<APlayerState>() : Cast<APlayerState>(PlayerState);
		if (PlayerState)
		{
			if (PlayerState->GetPingInMilliseconds() > HighPingThreshold)
			{
				HighPingWarning();
				PingAnimationRunningTime = 0.f;
				ServerReportPingStatus(true);
			}
			else
			{
				ServerReportPingStatus(false);
			}
		}
		HighPingRunningTime = 0.0f;
	}
	bool bHighPingAnimation = BlasterHUD && BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HighPingAnimation &&
		BlasterHUD->CharacterOverlay->IsAnimationPlaying(BlasterHUD->CharacterOverlay->HighPingAnimation);
	if (bHighPingAnimation)
	{
		PingAnimationRunningTime += DeltaTime;
		if (PingAnimationRunningTime > HighPingDuration)
		{
			StopHighPingWarning();

		}

	}
}
void ABlasterPlayerController::ShowRetunToMainMenu()
{

	if (MenuInGameWidgetClass == nullptr)
	{
		return;
	}
	if (MenuInGameWidget == nullptr)
	{
		MenuInGameWidget = CreateWidget<UMenuInGame>(this, MenuInGameWidgetClass);
	}
	if (MenuInGameWidget)
	{
		bMenuInGameWidgetVisible = !bMenuInGameWidgetVisible;
		if (bMenuInGameWidgetVisible)
		{
			MenuInGameWidget->MenuSetup();
		}
		else
		{
			MenuInGameWidget->MenuTeardown();
		}
	}
}

void ABlasterPlayerController::ShowOrHideChat()
{
	UE_LOG(LogTemp, Warning, TEXT("ShowOrHideChat"));
	if (ChatWidget)
	{
		bChatWidgetVisible = !bChatWidgetVisible;
		if (bChatWidgetVisible)
		{
			FInputModeGameAndUI InputModeGameAndUI;
			UE_LOG(LogTemp, Warning, TEXT("Show Chat"));
			ChatWidget->SetVisibility(ESlateVisibility::Visible);
			//Enter to ChatInputText
			ChatWidget->ChatInputText->SetUserFocus(GetWorld()->GetFirstPlayerController());
			SetInputMode(InputModeGameAndUI);
			SetShowMouseCursor(true);
			ChatWidget->ChatScrollBox->ScrollToEnd();


		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Hide Chat"));
			FInputModeGameOnly InputModeGameOnly;
			ChatWidget->SetVisibility(ESlateVisibility::Hidden);
			//Leave from ChatInputText
			SetInputMode(InputModeGameOnly);
			SetShowMouseCursor(false);

		}
	}
}

void ABlasterPlayerController::OnRep_ShowTeamScores()
{
	if (bShowTeamScores)
	{
		InitTeamScores();
	}
	else
	{
		HideTeamScores();
	}

}


void ABlasterPlayerController::ServerReportPingStatus_Implementation(bool bHighPing)
{
	HighPingDelegate.Broadcast(bHighPing);
}
void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerController, MatchState);
	DOREPLIFETIME(ABlasterPlayerController, bShowTeamScores);
}
void ABlasterPlayerController::HideTeamScores()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD && BlasterHUD->CharacterOverlay)
	{
		BlasterHUD->CharacterOverlay->BlueTeamScoreBar->SetVisibility(ESlateVisibility::Hidden);
		BlasterHUD->CharacterOverlay->RedTeamScoreBar->SetVisibility(ESlateVisibility::Hidden);
		BlasterHUD->CharacterOverlay->BlueTeamScoreText->SetVisibility(ESlateVisibility::Hidden);
		BlasterHUD->CharacterOverlay->RedTeamScoreText->SetVisibility(ESlateVisibility::Hidden);
	}
}
void ABlasterPlayerController::InitTeamScores()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD && BlasterHUD->CharacterOverlay)
	{
		BlasterHUD->CharacterOverlay->BlueTeamScoreBar->SetVisibility(ESlateVisibility::Visible);
		BlasterHUD->CharacterOverlay->RedTeamScoreBar->SetVisibility(ESlateVisibility::Visible);
		BlasterHUD->CharacterOverlay->BlueTeamScoreText->SetVisibility(ESlateVisibility::Visible);
		BlasterHUD->CharacterOverlay->RedTeamScoreText->SetVisibility(ESlateVisibility::Visible);
		BlasterHUD->CharacterOverlay->BlueTeamScoreBar->SetPercent(0.f);
		BlasterHUD->CharacterOverlay->RedTeamScoreBar->SetPercent(0.f);
		BlasterHUD->CharacterOverlay->BlueTeamScoreText->SetText(FText::FromString("0"));
		BlasterHUD->CharacterOverlay->RedTeamScoreText->SetText(FText::FromString("0"));
	}
}
void ABlasterPlayerController::SetHUDBlueTeamScore(float Score, float MaxScore)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD && BlasterHUD->CharacterOverlay)
	{
		const float ScorePercentage = Score / MaxScore;
		BlasterHUD->CharacterOverlay->BlueTeamScoreBar->SetPercent(ScorePercentage);
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		BlasterHUD->CharacterOverlay->BlueTeamScoreText->SetText(FText::FromString(ScoreText));
	}
}
void ABlasterPlayerController::SetHUDRedTeamScore(float Score, float MaxScore)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD && BlasterHUD->CharacterOverlay)
	{
		const float ScorePercentage = Score / MaxScore;
		BlasterHUD->CharacterOverlay->RedTeamScoreBar->SetPercent(ScorePercentage);
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));
		BlasterHUD->CharacterOverlay->RedTeamScoreText->SetText(FText::FromString(ScoreText));
	}

}
void ABlasterPlayerController::CheckTimeSync(float deltaTime)
{
	TimeSyncRunningTime += deltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.0f;
	}
}
float ABlasterPlayerController::GetServerTime()
{
	if (HasAuthority())
	{
		return GetWorld()->GetTimeSeconds();		
	}
	else
	{
		return GetWorld()->GetTimeSeconds() + ClientServerDelta;
	}
}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlasterPlayerController::OnMatchStateSet(FName State , bool bTeamGame)
{
	MatchState = State;
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted(bTeamGame);
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
	
}
void ABlasterPlayerController::HandleMatchHasStarted(bool bTeamGame)
{
	if (HasAuthority())
	{
		bShowTeamScores = bTeamGame;
	}
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		if (BlasterHUD->CharacterOverlay == nullptr) 
		{
			BlasterHUD->AddCharacterOverlay();
		}
		if (BlasterHUD->AnnouncementOverlay == nullptr)
		{
			BlasterHUD->AddAnnouncementOverlay();
		}
		if (BlasterHUD->AnnouncementOverlay)
		{
			BlasterHUD->AnnouncementOverlay->SetVisibility(ESlateVisibility::Hidden);
		}
		if (bTeamGame)
		{
			InitTeamScores();
		}
		else
		{
			HideTeamScores();
		}
	}
}
void ABlasterPlayerController::HandleCooldown()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
        if (BlasterHUD->CharacterOverlay)
        {
            BlasterHUD->CharacterOverlay->RemoveFromParent();
        }
		bool bHUDValid = BlasterHUD->AnnouncementOverlay &&
			BlasterHUD->AnnouncementOverlay->AnnouncementText &&
			BlasterHUD->AnnouncementOverlay->InfoText;

		if (bHUDValid)
		{
			BlasterHUD->AnnouncementOverlay->SetVisibility(ESlateVisibility::Visible);
			FString AnnounceText = Announcement::NewMatchStartsIn;
			//BlasterHUD->AnnouncementOverlay->InfoText->SetText(FText());

			BlasterHUD->AnnouncementOverlay->AnnouncementText->SetText(FText::FromString(AnnounceText));

			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
			if (BlasterGameState && BlasterPlayerState)
			{
				TArray<ABlasterPlayerState*> TopScoringPlayers = BlasterGameState->TopScoringPlayers;
				FLinearColor colorText;
				FString InfoTextString = bShowTeamScores ? GetTeamInfoText(BlasterGameState, colorText) : GetInfoText(TopScoringPlayers, colorText);

				BlasterHUD->AnnouncementOverlay->InfoText->SetText(FText::FromString(InfoTextString));
			
				BlasterHUD->AnnouncementOverlay->InfoText->SetColorAndOpacity(colorText);
				//BlasterHUD->AnnouncementOverlay->AnnouncementText->SetText(FText::FromString(AnnounceText));
			}
		}

	}
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	if (BlasterCharacter && BlasterCharacter->GetCombatComponent())
	{
		BlasterCharacter->bDisableGameplayInput = true;
		BlasterCharacter->GetCombatComponent()->FireButtonPressed(false);
	}

}
FString ABlasterPlayerController::GetInfoText(const TArray<ABlasterPlayerState*>& PlayerStates ,FLinearColor& colorText)
{
	ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
	colorText = FLinearColor::White;
	if (BlasterPlayerState == nullptr)
	{
		return FString();
	}
	FString InfoTextString;
	if (PlayerStates.Num() == 0)
	{
		InfoTextString = Announcement::ThereIsNoWinner;
	}
	else if (PlayerStates.Num() == 1 && PlayerStates[0] == BlasterPlayerState)
	{
		InfoTextString = Announcement::YouAreTheWinner;
	}
	else if (PlayerStates.Num() == 1)
	{
		InfoTextString = FString::Printf(TEXT("%s is the winner!"), *PlayerStates[0]->GetPlayerName());
	}
	else if (PlayerStates.Num() > 1)
	{
		InfoTextString = Announcement::ThereIsATieBetween;
		InfoTextString.Append("\n");
		for (auto TiedPlayer : PlayerStates)
		{
			InfoTextString.Append(FString::Printf(TEXT("%s \n"), *TiedPlayer->GetPlayerName()));
		}
	}
	return InfoTextString;
}

FString ABlasterPlayerController::GetTeamInfoText(ABlasterGameState* BlasterGameState, FLinearColor& colorText)
{
	if (BlasterGameState == nullptr)
	{
		return FString();
	}
	FString InfoTextString;
	const int32 BlueTeamScore = BlasterGameState->BlueTeamScore;
	const int32 RedTeamScore = BlasterGameState->RedTeamScore;
	colorText = FLinearColor::White;

	if (RedTeamScore == 0 && BlueTeamScore == 0)
	{
		InfoTextString = Announcement::ThereIsNoWinner;
	}
	else if (RedTeamScore > BlueTeamScore)
	{
		InfoTextString = Announcement::RedTeamIsTheWinner;
		colorText = FLinearColor::Red;
	}
	else if (BlueTeamScore > RedTeamScore)
	{
		InfoTextString = Announcement::BlueTeamIsTheWinner;
		colorText = FLinearColor::Blue;
	}
	else if (BlueTeamScore == RedTeamScore)
	{
		InfoTextString = Announcement::ThereIsATieBetweenTheTeams;
	}

	return InfoTextString;
}

void ABlasterPlayerController::HighPingWarning()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->HighPingImage && BlasterHUD->CharacterOverlay->HighPingAnimation;
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->HighPingImage->SetOpacity(1.f);
		BlasterHUD->CharacterOverlay->PlayAnimation(BlasterHUD->CharacterOverlay->HighPingAnimation,0.f,5);
	}

}
void ABlasterPlayerController::StopHighPingWarning()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD && BlasterHUD->CharacterOverlay && BlasterHUD->CharacterOverlay->HighPingImage && BlasterHUD->CharacterOverlay->HighPingAnimation;
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->HighPingImage->SetOpacity(0.f);
		if (BlasterHUD->CharacterOverlay->IsAnimationPlaying(BlasterHUD->CharacterOverlay->HighPingAnimation))
		{
			BlasterHUD->CharacterOverlay->StopAnimation(BlasterHUD->CharacterOverlay->HighPingAnimation);
		}		
	}
}

void ABlasterPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime, bool bIsTeamsMatch)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	bShowTeamScores = bIsTeamsMatch;
	OnMatchStateSet(MatchState, bIsTeamsMatch);
	
	if (BlasterHUD && MatchState == MatchState::WaitingToStart)
	{
		BlasterHUD->AddAnnouncementOverlay();
	}
}

void ABlasterPlayerController::ServerCheckMatchState_Implementation()
{
	ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		bShowTeamScores = GameMode->bTeamMatch;
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime,LevelStartingTime, bShowTeamScores);
	}

}
void ABlasterPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted(bShowTeamScores);
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
	
}

void ABlasterPlayerController::OnChatMessageSent(const FText& Message, ETextCommit::Type CommitMethod)
{

	if (CommitMethod == ETextCommit::OnEnter)
	{
		FString PlayerName = GetPlayerState<APlayerState>()->GetPlayerName();
		//Check if Message is a command /spec
		if (Message.ToString() == "/spec")
		{
				ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
				if (BlasterCharacter)
				{
					BlasterCharacter->GetCombatComponent()->RemoveCrosshair();
					BlasterCharacter->RagdollDeath(false);
					//Disable CharacterOverlay
					BlasterHUD->CharacterOverlay->SetVisibility(ESlateVisibility::Hidden);
					//Disable Chat
					ChatWidget->SetVisibility(ESlateVisibility::Hidden);
					//remove all widgets
					UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
					//Set Focus to game
					FInputModeGameOnly InputModeGameOnly;
					SetInputMode(InputModeGameOnly);
					SetShowMouseCursor(false);



					ServerChangeState(BlasterCharacter);
					if (!HasAuthority())
					{
						ClientChangeState(BlasterCharacter);
					}
					
					//BlasterCharacter->GetController()->ChangeState(NAME_Spectating);
				}
				
			
		}
		else
		{
			ServerChatCommitted(Message, PlayerName);

			//Clear ChatInputText
			ChatWidget->ChatInputText->SetText(FText());
			//Leave from ChatInputText
			ShowOrHideChat();
		}
	}
}

void ABlasterPlayerController::MulticastRestartMap_Implementation()
{
	// Obtenez le nom du niveau actuel
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this);

	// Redémarrez le niveau
	UGameplayStatics::OpenLevel(this, *CurrentLevelName);
}

void ABlasterPlayerController::ClientChangeState_Implementation(ABlasterCharacter* BlasterCharacter)
{
	if (BlasterCharacter != nullptr)
	{
		BlasterCharacter->GetController()->ChangeState(NAME_Spectating);
	}
}

void ABlasterPlayerController::ServerChangeState_Implementation(ABlasterCharacter* BlasterCharacter)
{
	if (BlasterCharacter != nullptr)
	{
		BlasterCharacter->GetController()->ChangeState(NAME_Spectating);
	}
}
bool ABlasterPlayerController::ServerChangeState_Validate(ABlasterCharacter* BlasterCharacter)
{
	return true;
}

void ABlasterPlayerController::ServerChatCommitted_Implementation(const FText& Text, const FString& PlayerName)
{
	BlasterGameMode->SendChatMsg(Text, PlayerName);
}
void ABlasterPlayerController::ClientChatCommitted_Implementation(const FText& Text, const FString& PlayerName)
{
	if (ChatWidget)
	{
		ChatWidget->AddChatMessage(Text, PlayerName);
	}
}



void ABlasterPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (BlasterHUD  && BlasterHUD->CharacterOverlay)
		{
			CharacterOverlay = BlasterHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				if (bInitializeHealth)
				{
					SetHUDHealth(HUDHealth, HUDMaxHealth);
				}
				if (bInitializeShield)
				{
					SetHUDShield(HUDShield, HUDMaxShield);
				}
				if (bInitializeScore)
				{
					SetHUDScore(HUDScore);
				}
				if (bInitializeDefeats)
				{
					SetHUDDefeats(HUDDefeats);
				}
				if (bInitializeWeaponAmmo)
				{
					SetHUDWeaponAmmo(HUDWeaponAmmo);
				}
				if (bInitializeCarriedAmmo)
				{
					SetHUDCarriedAmmo(HUDCarriedAmmo);
				}
				ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
				if (BlasterCharacter && BlasterCharacter->GetCombatComponent())
				{
					if (bInitializeGrenadeAmount)
					{
						SetHUDGrenadeAmount(BlasterCharacter->GetCombatComponent()->GetGrenades());
					}
					
				}
			}
		}
	}

}

void ABlasterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent == nullptr) return;
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MenuAction, ETriggerEvent::Completed, this, &ABlasterPlayerController::ShowRetunToMainMenu);
		EnhancedInputComponent->BindAction(ChatAction, ETriggerEvent::Completed, this, &ABlasterPlayerController::ShowOrHideChat);
	}
}

void ABlasterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}


void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	SingleTripTime = 0.5f * RoundTripTime ;
	float CurrentServerTime = TimeServerReceivedRequest + SingleTripTime;
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}
