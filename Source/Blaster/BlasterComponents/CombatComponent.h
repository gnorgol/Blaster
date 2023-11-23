// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Blaster/HUD/BlasterHUD.h"
#include "Blaster/Weapon/WeaponTypes.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "CombatComponent.generated.h"



class AWeapon;
class ABlasterCharacter;
class ABlasterPlayerController;
class ABlasterHUD;
class AProjectile;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	friend class ABlasterCharacter;

	void EquipWeapon(AWeapon* WeaponToEquip);
	void SwapWeapon();

	
	void Reload();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	void FireButtonPressed(bool bPressed);

	UFUNCTION(BlueprintCallable)
	void ShotGunShellReloaded();

	void JumpToShotgunEndReload();

	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();
	UFUNCTION(BlueprintCallable)
	void FinishSwap();
	UFUNCTION(BlueprintCallable)
	void FinishSwapAttachWeapon();

	UFUNCTION(BlueprintCallable)
		void LaunchGrenade();
	UFUNCTION(Server, Reliable)
		void ServerLaunchGrenade(const FVector_NetQuantize& Target);

	void PickUpAmmo(EWeaponType WeaponType, int32 AmmoAmount);
	bool bLocallyReloading = false;
	void RemoveCrosshair();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);
	void SetSprinting(bool bIsSprinting);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);
	UFUNCTION(Server, Reliable)
		void ServerSetSprinting(bool bIsSprinting);

	UFUNCTION()
		void OnRep_EquippedWeapon();
	UFUNCTION()
		void OnRep_SecondaryWeapon();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire(const FVector_NetQuantize& TraceHitTraget,float FireDelay);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTraget);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTraget,float FireDelay);

	UFUNCTION(NetMulticast, Reliable)
		void MulticastShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTraget);

	void TraceUnderCrosshair(FHitResult& TraceHitResult);

	void SetHUDCrosshair(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();
	int32 AmountNeededToReload();

	void ThrowGrenade();

	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> GrenadeClass;

	void DropEquippedWeapon();
	void AttacheActorToRightHand(AActor* ActorToAttach);
	void AttachActorToLeftHand(AActor* ActorToAttach);
	void AttachActorToBack(AActor* ActorToAttach);
	void ReloadEmptyWeapon();

	void PlayEquipWeaponSound(AWeapon* WeaponToEquip);

	void UpdateCarriedAmmo();
	void ShowAttachedGrenade(bool bShow);
	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);

private:
	UPROPERTY()
	ABlasterCharacter* Character;
	UPROPERTY()
	ABlasterPlayerController* PlayerController;
	UPROPERTY()
	ABlasterHUD* HUD;
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	AWeapon* SecondaryWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming = false;

	bool bAimingButtonPressed = false;

	UFUNCTION()
	void OnRep_Aiming();

	UPROPERTY(ReplicatedUsing = OnRep_Sprinting)
	bool bIsSpriting = false;

	bool bSprintingButtonPressed = false;

	UFUNCTION()
		void OnRep_Sprinting();


	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimingWalkSpeed;
	UPROPERTY(EditAnywhere)
		float SprintingSpeed;

	bool bFireButtonPressed;

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshairAimFactor;
	float CrosshairShootingFactor;

	FVector HitTarget;

	FHUDPackage HUDPackage;

	// Field of View when not aiming
	float DefaultFOV;

	UPROPERTY(EditAnywhere, Category = "Combat")
		float ZoomedFOV = 30.0f;

	float CurrentFOV;
	void InterpFOV(float DeltaTime);

	FTimerHandle FireTimerHandle;



	bool bCanFire = true;
		
	void StartFireTimer();
	void FireTimerFinished();
	void Fire();
	void FireProjectileWeapon();
	void FireHitscanWeapon();
	void FireShotgunWeapon();
	void LocalFire(FVector_NetQuantize TraceHitTarget);
	void LocalShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTarget);

	bool CanFire();

	// Carried Ammo for the current weapon equipped
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere, Category = "Combat")
		int32 MaxCarriedAmmo = 300;

	UPROPERTY(EditAnywhere, Category = "Combat")
		int32 StartingARAmmo = 30;
	UPROPERTY(EditAnywhere)
		int32 StartingRocketAmmo = 0;
	UPROPERTY(EditAnywhere)
		int32 StartingPistolAmmo = 0;
	UPROPERTY(EditAnywhere)
		int32 StartingSMGAmmo = 0;
	UPROPERTY(EditAnywhere)
		int32 StartingShotgunAmmo = 0;
	UPROPERTY(EditAnywhere)
		int32 StartingSniperAmmo = 0;
	UPROPERTY(EditAnywhere)
		int32 StartingGrenadeLauncherAmmo = 0;
	void InitializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
		ECombatState CombatState = ECombatState::ECS_Unoccupied;

	UFUNCTION()
		void OnRep_CombatState();

	void UpdateAmmoValues();
	void UpdateShotgunAmmoValues();

	

	UPROPERTY(ReplicatedUsing = OnRep_GrenadeAmount)
	int32 GrenadeAmount = 3;
	UPROPERTY(EditAnywhere)
	int32 MaxGrenadeAmount = 3;
	UFUNCTION()
	void OnRep_GrenadeAmount();
	void UpdateHUDGrenadeAmount();

public:
	FORCEINLINE int32 GetGrenades() const { return GrenadeAmount; }
	bool ShouldSwapWeapon();

};
  