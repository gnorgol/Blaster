// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlasterGameState.generated.h"

/**
 * 
 */
class ABlasterPlayerState;
UCLASS()
class BLASTER_API ABlasterGameState : public AGameState
{
	GENERATED_BODY()
public:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScoringPlayers(ABlasterPlayerState* ScoringPlayer);

	// Free for all score
	UPROPERTY(Replicated)
	TArray<ABlasterPlayerState*> TopScoringPlayers;

	//Team 

	TArray<ABlasterPlayerState*> RedTeam;
	TArray<ABlasterPlayerState*> BlueTeam;

	//Team Score
	void SetRedTeamScore();
	void SetBlueTeamScore();


	UPROPERTY(ReplicatedUsing = OnRep_RedTeamScore)
		float RedTeamScore = 0;
	UFUNCTION()
		void OnRep_RedTeamScore();
	UPROPERTY(ReplicatedUsing = OnRep_BlueTeamScore)
		float BlueTeamScore = 0;
	UFUNCTION()
		void OnRep_BlueTeamScore();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team Score")
		float TeamScoreMax = 10;





private:

	UPROPERTY(Replicated)
	float TopScore = 0;
};
