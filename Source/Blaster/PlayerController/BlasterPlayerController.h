// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);

/**
 * 
 */
class ABlasterHUD;
class UCharacterOverlay;
class ABlasterGameMode;
class UInputAction;
class ABlasterGameState;
class ABlasterPlayerState;

UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDKillFieldInfo(const FString& KillerName, const FString& VictimName);
	void SetHUDKillFieldPlayerInfo(const FString& PlayerName,bool bIsDead);
	void SetHUDMatchCountdown(float Countdown);
	void SetHUDWarmupCountdown(float Countdown);
	void SetHUDGrenadeAmount(int32 Amount);
	void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTimes) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void HideTeamScores();
	void InitTeamScores();
	void SetHUDBlueTeamScore(float Score,float MaxScore);
	void SetHUDRedTeamScore(float Score, float MaxScore);

	virtual float GetServerTime(); // Sync time between server and client
	virtual void ReceivedPlayer() override; 
	void OnMatchStateSet(FName State, bool bTeamGame);

	float SingleTripTime = 0.0f; // Time it takes for a packet to go from client to server and back

	FHighPingDelegate HighPingDelegate;

	void BrodcastKillFeed(APlayerState* Killer, APlayerState* Victim , EWeaponType WeaponTypeUsed);
protected:
	virtual void BeginPlay() override;
	void SetHUDMatchTime();
	void PollInit();
	virtual void SetupInputComponent() ;
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
	void HandleMatchHasStarted(bool bTeamGame);
	void HandleCooldown();
	UFUNCTION(Server , Reliable)
		void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
		void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match,float Cooldown, float StartingTime, bool bIsTeamsMatch);

	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaTime);
	UFUNCTION(Server, Reliable)
		void ServerReportPingStatus(bool bHighPing);
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* MenuAction;
	void ShowRetunToMainMenu();
	UFUNCTION(Client, Reliable)
	void ClientKillFeed(APlayerState* Killer, APlayerState* Victim, EWeaponType WeaponTypeUsed);


	UPROPERTY(ReplicatedUsing = OnRep_ShowTeamScores)
	bool bShowTeamScores = false;

	UFUNCTION()
	void OnRep_ShowTeamScores();

	FString GetInfoText (const TArray<ABlasterPlayerState*>& PlayerStates , FLinearColor& colorText);
	FString GetTeamInfoText(ABlasterGameState* BlasterGameState, FLinearColor& colorText);
	
private:
	UPROPERTY()
	ABlasterHUD* BlasterHUD;

	float LevelStartingTime = 0.f;
	float MatchTime = 0.f;
	float WarmupTime = 0.f;
	float CooldownTime = 0.f;
	uint32 CountdownInt = 0;


	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();

	UPROPERTY()
	UCharacterOverlay* CharacterOverlay;

	UPROPERTY()
	ABlasterGameMode* BlasterGameMode;

	bool bInitializeHealth = false;
	float HUDHealth;
	float HUDMaxHealth;
	bool bInitializeShield = false;
	float HUDShield;
	float HUDMaxShield;
	bool bInitializeScore = false;
	float HUDScore;
	bool bInitializeDefeats = false;
	int32 HUDDefeats;
	bool bInitializeGrenadeAmount = false;
	int32 HUDGrenadeAmount;
	float HUDCarriedAmmo;
	bool bInitializeCarriedAmmo = false;
	float HUDWeaponAmmo;
	bool bInitializeWeaponAmmo = false;

	float HighPingRunningTime = 0.f;
	float PingAnimationRunningTime = 0.f;

	UPROPERTY(EditAnywhere, Category = "Ping")
	float HighPingDuration = 5.f;

	

	UPROPERTY(EditAnywhere, Category = "Ping")
	float CheckPingFrequency = 20.f;


	UPROPERTY(EditAnywhere, Category = "Ping")
	float HighPingThreshold = 50.f;

	UPROPERTY(EditAnywhere, Category = HUD)
	TSubclassOf<class UUserWidget> ReturnToMainMenuWidgetClass;

	UPROPERTY()
		class UReturnToMainMenu* ReturnToMainMenuWidget;

	bool bReturnToMainMenuWidgetVisible = false;
	
};
