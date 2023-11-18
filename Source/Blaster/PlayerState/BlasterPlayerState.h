// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Blaster/BlasterTypes/Team.h"
#include "BlasterPlayerState.generated.h"

/**
 * 
 */
class ABlasterPlayerController;
class ABlasterCharacter;
UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Score() override;
	UFUNCTION()
	virtual void OnRep_KillName();
	UFUNCTION()
	virtual void OnRep_KilledBy();
	
	UFUNCTION()
	virtual void OnRep_Defeats();	

	UFUNCTION(NetMulticast, Reliable)
		void MulticastUpdateKillFieldHUD();

	void UpdateKillFieldHUD(FString KillerName, FString VictimName);

	void AddToScore(float ScoreAmount);
	void AddToDefeats(int32 DefeatsAmount);
	void SetKillName(FString Name);
	void SetKilledBy(FString Name);
	void UpdateKillNameMessageHUD();
	void UpdateKilledByMessageHUD();
	void clearKillName();

private:
	UPROPERTY()
	ABlasterPlayerController* Controller;
	UPROPERTY()
	ABlasterCharacter* Character;

	UPROPERTY(ReplicatedUsing = OnRep_Defeats)
	int32 Defeats;

	UPROPERTY(ReplicatedUsing = OnRep_KillName)
		FString KillName;

	UPROPERTY(ReplicatedUsing = OnRep_KilledBy)
		FString KilledBy;

	UPROPERTY(Replicated)
		FText ListOfKills;

	UPROPERTY(ReplicatedUsing = OnRep_Team)
		ETeam Team = ETeam::ET_NoTeam;
	UFUNCTION()
	void OnRep_Team();

public:
	FORCEINLINE ETeam GetTeam() const { return Team; }
	void SetTeam(ETeam NewTeam);

	
};
