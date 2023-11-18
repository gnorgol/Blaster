// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamsGameMode.h"
#include "Blaster/GameState/BlasterGameState.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "Kismet/GameplayStatics.h"

void ATeamsGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	ABlasterGameState* BGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		for (auto PState : BGameState->PlayerArray)
		{
			ABlasterPlayerState* BPState = Cast<ABlasterPlayerState>(PState.Get());
			if (BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (BGameState->BlueTeam.Num() >= BGameState->RedTeam.Num())
				{
					BPState->SetTeam(ETeam::ET_RedTeam);
					BGameState->RedTeam.AddUnique(BPState);
				}
				else
				{
					BPState->SetTeam(ETeam::ET_BlueTeam);
					BGameState->BlueTeam.AddUnique(BPState);
				}
			}
		}
	}
}

void ATeamsGameMode::PostLogin(APlayerController* NewPlayer)
{
Super::PostLogin(NewPlayer);

	ABlasterGameState* BGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		ABlasterPlayerState* BPState = Cast<ABlasterPlayerState>(NewPlayer->PlayerState);
		if (BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
		{
			if (BGameState->BlueTeam.Num() >= BGameState->RedTeam.Num())
			{
				BPState->SetTeam(ETeam::ET_RedTeam);
				BGameState->RedTeam.AddUnique(BPState);
			}
			else
			{
				BPState->SetTeam(ETeam::ET_BlueTeam);
				BGameState->BlueTeam.AddUnique(BPState);
			}
		}
	}

}

void ATeamsGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	ABlasterGameState* BGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		ABlasterPlayerState* BPState = Cast<ABlasterPlayerState>(Exiting->PlayerState);
		if (BPState)
		{
			if (BGameState->RedTeam.Contains(BPState))
			{
				BGameState->RedTeam.Remove(BPState);
			}
			else if (BGameState->BlueTeam.Contains(BPState))
			{
				BGameState->BlueTeam.Remove(BPState);
			}
		}
	}
}
