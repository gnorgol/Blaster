// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "BlasterGameMode.generated.h"

/**
 * 
 */
class ABlasterCharacter;
class ABlasterPlayerController;
UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ABlasterGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void PlayerEliminated(ABlasterCharacter* EliminatedPlayer, ABlasterPlayerController* VictimController, ABlasterPlayerController* Killer);
	virtual void RequestRespawn(ACharacter* CharacterToRespawn , AController* Controller);

	UPROPERTY(EditDefaultsOnly, Category = "Time")
		float WarmupTime = 5.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Time")
		float MatchTime = 120.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Time")
		float LevelStartingTime = 0.f;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;
private:
	float CountdownTime = 0.0f;
};
