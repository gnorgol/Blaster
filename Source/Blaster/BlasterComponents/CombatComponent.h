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
		void LaunchGrenade();
	UFUNCTION(Server, Reliable)
		void ServerLaunchGrenade(const FVector_NetQuantize& Target);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
		void OnRep_EquippedWeapon();
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTraget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTraget);

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
	void ReloadEmptyWeapon();

	void PlayEquipWeaponSound();

	void UpdateCarriedAmmo();
	void ShowAttachedGrenade(bool bShow);

private:
	UPROPERTY()
	ABlasterCharacter* Character;
	UPROPERTY()
	ABlasterPlayerController* PlayerController;
	UPROPERTY()
	ABlasterHUD* HUD;
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float AimingWalkSpeed;

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

	bool CanFire();

	// Carried Ammo for the current weapon equipped
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

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


};
  