// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameState.h"
#include "Net/UnrealNetwork.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include <Blaster/GameMode/BlasterGameMode.h>

void ABlasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlasterGameState, TopScoringPlayers);
	DOREPLIFETIME(ABlasterGameState, RedTeamScore);
	DOREPLIFETIME(ABlasterGameState, BlueTeamScore);
}

void ABlasterGameState::UpdateTopScoringPlayers(ABlasterPlayerState* ScoringPlayer)
{
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if (ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();

	}

}

void ABlasterGameState::SetRedTeamScore()
{
	RedTeamScore = FMath::Clamp(RedTeamScore + 1, 0.0f, TeamScoreMax);

	ABlasterPlayerController* BPlayer = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDRedTeamScore(RedTeamScore, TeamScoreMax);
	}
	if (RedTeamScore >= TeamScoreMax)
	{
		UE_LOG(LogTemp, Warning, TEXT("Red Team Win"));
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ABlasterGameState::SetMatchWinner, 1.5f, false);





	}
}

void ABlasterGameState::SetBlueTeamScore()
{
	BlueTeamScore = FMath::Clamp(BlueTeamScore + 1, 0.0f, TeamScoreMax);

	ABlasterPlayerController* BPlayer = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDBlueTeamScore(BlueTeamScore, TeamScoreMax);
	}
	if (BlueTeamScore >= TeamScoreMax)
	{
		UE_LOG(LogTemp, Warning, TEXT("Blue Team Win"));
		//win blue team
		//get Blaster Game Mode
		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(TimerHandle, this, &ABlasterGameState::SetMatchWinner, 1.5f, false);

	}
}

void ABlasterGameState::OnRep_RedTeamScore()
{
	ABlasterPlayerController* BPlayer = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDRedTeamScore(RedTeamScore, TeamScoreMax);
	}

}

void ABlasterGameState::OnRep_BlueTeamScore()
{
	ABlasterPlayerController* BPlayer = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayer)
	{
		BPlayer->SetHUDBlueTeamScore(BlueTeamScore, TeamScoreMax);
	}
}

void ABlasterGameState::SetMatchWinner()
{
	ABlasterGameMode* BGameMode = Cast<ABlasterGameMode>(GetWorld()->GetAuthGameMode());
	if (BGameMode)
	{
		BGameMode->SetMatchState(MatchState::Cooldown);
	}
}
