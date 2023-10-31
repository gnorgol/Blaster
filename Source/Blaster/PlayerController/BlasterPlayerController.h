// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */
class ABlasterHUD;
class UCharacterOverlay;
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDKillFieldInfo(const FString& KillerName, const FString& VictimName);
	void SetHUDKillFieldPlayerInfo(const FString& PlayerName,bool bIsDead);
	void SetHUDMatchCountdown(float Countdown);
	void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTimes) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual float GetServerTime(); // Sync time between server and client
	virtual void ReceivedPlayer() override; 
	void OnMatchStateSet(FName State);

	

	// Sync time between server and client

	UFUNCTION(Server, Reliable)
		void ServerRequestServerTime(float TimeOfClientRequest);

	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedRequest);

	float ClientServerDelta = 0.0f; // Difference between client and server time

	UPROPERTY(EditAnywhere, Category = "Time")
	float TimeSyncFrequency = 5.0f; // How often to sync time between client and server

	UPROPERTY(EditAnywhere, Category = "Time")
		float TimeSyncRunningTime = 0.0f;
	void CheckTimeSync(float deltaTime);


protected:
	virtual void BeginPlay() override;
	void SetHUDMatchTime();
	void PollInit();
	void HandleMatchHasStarted();
private:
	UPROPERTY()
	ABlasterHUD* BlasterHUD;

	float MatchTime = 120.0f;
	uint32 CountdownInt = 0;


	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	UCharacterOverlay* CharacterOverlay;

	bool bInitializedCharacterOverlay = false;

	float HUDHealth;
	float HUDMaxHealth;
	float HUDScore;
	int32 HUDDefeats;
	
};
