// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* EliminatedPlayer, ABlasterPlayerController* VictimController, ABlasterPlayerController* Killer)
{
	if (EliminatedPlayer)
	{
		EliminatedPlayer->RagdollDeath();
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
