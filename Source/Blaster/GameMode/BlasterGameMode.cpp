// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Blaster/GameState/BlasterGameState.h"


namespace MatchState
{
	const FName Cooldown = FName("Cooldown"); 

}




ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}
void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStartingTime = GetWorld()->GetTimeSeconds();
}
void ABlasterGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		ABlasterPlayerController* PlayerController = Cast<ABlasterPlayerController>(*It);
		if (PlayerController)
		{
			PlayerController->OnMatchStateSet(MatchState);
		}

	}

}
void ABlasterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.0f)
		{
			StartMatch();
		}

	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.0f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime =  CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.0f)
		{
			RestartGame();
		}
	}
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* EliminatedPlayer, ABlasterPlayerController* VictimController, ABlasterPlayerController* Killer)
{
	ABlasterPlayerState* KillerPlayerState = Killer ? Cast<ABlasterPlayerState>(Killer->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;
	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();

	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
		VictimPlayerState->SetKilledBy(KillerPlayerState->GetPlayerName());
		//clear killed by after 5 seconds
		FTimerHandle KilledByTimer;
		GetWorldTimerManager().SetTimer(KilledByTimer, [VictimPlayerState]() {VictimPlayerState->SetKilledBy(""); }, 5.f, false);
		//VictimPlayerState->UpdateKilledByFieldHUD(VictimPlayerState->GetPlayerName(), KillerPlayerState->GetPlayerName());
	}
	if (KillerPlayerState && KillerPlayerState != VictimPlayerState && BlasterGameState)
	{
		KillerPlayerState->AddToScore(1.f);
		BlasterGameState->UpdateTopScoringPlayers(KillerPlayerState);
		KillerPlayerState->SetKillName(VictimPlayerState->GetPlayerName());
		//clear kill after 5 seconds
		FTimerHandle KillTimer;
		GetWorldTimerManager().SetTimer(KillTimer, [KillerPlayerState]() {KillerPlayerState->SetKillName(""); }, 5.f, false);
		//KillerPlayerState->UpdateKillFieldHUD(KillerPlayerState->GetPlayerName(), VictimPlayerState->GetPlayerName());
	}
	ABlasterCharacter* KillerCharacter = Killer ? Cast<ABlasterCharacter>(Killer->GetPawn()) : nullptr;

	if (EliminatedPlayer)
	{
		EliminatedPlayer->RagdollDeath();
		EliminatedPlayer->SetKiller(KillerCharacter);		
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* CharacterToRespawn, AController* Controller)
{
	if (CharacterToRespawn)
	{
		CharacterToRespawn->Reset();
		CharacterToRespawn->Destroy();
	}
	if (Controller)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this,APlayerStart::StaticClass(),PlayerStarts);
		int32 RandomIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);
		
		RestartPlayerAtPlayerStart(Controller, PlayerStarts[RandomIndex]);

	}
}


