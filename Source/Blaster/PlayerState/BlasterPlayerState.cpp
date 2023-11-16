// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
	
}

void ABlasterPlayerState::OnRep_KillName()
{
	UpdateKillNameMessageHUD();
}

void ABlasterPlayerState::OnRep_KilledBy()
{
	UpdateKilledByMessageHUD();
}

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, Defeats);
	DOREPLIFETIME(ABlasterPlayerState, KillName);
	DOREPLIFETIME(ABlasterPlayerState, KilledBy);

}

void ABlasterPlayerState::UpdateKillFieldHUD(FString KillerName, FString VictimName)
{
	KilledBy = KillerName;
	KillName = VictimName;
	MulticastUpdateKillFieldHUD();
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore() + ScoreAmount);
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ABlasterPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}
void ABlasterPlayerState::SetKillName(FString Name)
{
	KillName = Name;	
	UpdateKillNameMessageHUD();
}
void ABlasterPlayerState::SetKilledBy(FString Name)
{
	KilledBy = Name;
	UpdateKilledByMessageHUD();
}
void ABlasterPlayerState::UpdateKillNameMessageHUD()
{
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDKillFieldPlayerInfo(KillName,false);
		}
	}

}
void ABlasterPlayerState::UpdateKilledByMessageHUD()
{
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDKillFieldPlayerInfo(KilledBy,true);
		}
	}
}

void ABlasterPlayerState::clearKillName()
{
	KillName = "";
	KilledBy = "";
}

void ABlasterPlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void ABlasterPlayerState::MulticastUpdateKillFieldHUD_Implementation()
{
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDKillFieldInfo(KillName, KilledBy);
		}
	}
}

