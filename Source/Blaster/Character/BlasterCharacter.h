// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "BlasterCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AWeapon;
class UCombatComponent;
class ULagCompensationComponent;
class UBuffComponent;
class UAnimMontage;
class ABlasterPlayerController;
class ABlasterPlayerState;
class UWidgetComponent;
class UBoxComponent;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABlasterCharacter();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);
	virtual void OnRep_ReplicatedMovement() override;
	virtual void Destroyed() override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEliminated();

	void RagdollDeath();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRagdollDeath();

	
	void PlayDeathMontage();
	void PlayReloadMontage();
	void PlayThrowGrenadeMontage();

	UPROPERTY(Replicated)
	bool bDisableGameplayInput = false;

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShow);

	void UpdateHUDHealth();
	void UpdateHUDShield();

	void UpdateHUDAmmo();

	void SpawnDefaultWeapon();

	UPROPERTY(EditAnywhere)
		UBoxComponent* head;
	UPROPERTY(EditAnywhere)
		UBoxComponent* pelvis;
	UPROPERTY(EditAnywhere)
		UBoxComponent* spine_02;
	UPROPERTY(EditAnywhere)
		UBoxComponent* spine_03;
	UPROPERTY(EditAnywhere)
		UBoxComponent* spine_04;
	UPROPERTY(EditAnywhere)
		UBoxComponent* spine_05;
	UPROPERTY(EditAnywhere)
		UBoxComponent* upperarm_l;
	UPROPERTY(EditAnywhere)
		UBoxComponent* upperarm_r;
	UPROPERTY(EditAnywhere)
		UBoxComponent* lowerarm_l;
	UPROPERTY(EditAnywhere)
		UBoxComponent* lowerarm_r;
	UPROPERTY(EditAnywhere)
		UBoxComponent* hand_l;
	UPROPERTY(EditAnywhere)
		UBoxComponent* hand_r;
	UPROPERTY(EditAnywhere)
		UBoxComponent* thigh_l;
	UPROPERTY(EditAnywhere)
		UBoxComponent* thigh_r;
	UPROPERTY(EditAnywhere)
		UBoxComponent* calf_l;
	UPROPERTY(EditAnywhere)
		UBoxComponent* calf_r;
	UPROPERTY(EditAnywhere)
		UBoxComponent* foot_l;
	UPROPERTY(EditAnywhere)
		UBoxComponent* foot_r;

	UPROPERTY()
		TMap<FName, UBoxComponent*> HitCollisionBoxes;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

	void Move(const FInputActionValue& Value);
	virtual void Jump() override;
	void Look(const FInputActionValue& Value);
	void Equip(const FInputActionValue& Value);
	void SwitchWeapon(const FInputActionValue& Value);
	void CrouchPressed(const FInputActionValue& Value);
	void AimPressed(const FInputActionValue& Value);
	void ReloadPressed(const FInputActionValue& Value);
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void PlayHitReactMontage();
	void SimProxiesTurn();
	void FirePressed(const FInputActionValue& Value);
	void ThrowGrenadePressed(const FInputActionValue& Value);
	void RotateInPlace(float DeltaTime);

	UFUNCTION()
	void ReceiveDamage(AActor* DamageActor, float DamageAmount, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputMappingContext* BlastCharacterMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* CrouchAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* EquipAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* FireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* ReloadAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* ThrowGrenadeAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		UInputAction* SwitchWeaponAction;

	void HideCameraIfCharacterCloseToWall();
	// Poll for any classes and initialize HUD
	void PollInit();

	void DropOrDestroyWeapon(AWeapon* Weapon);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	bool bHitCharacter = false;
	UFUNCTION()
	virtual void OnRep_Killer();

	void SetKiller(ABlasterCharacter* Character);

	FORCEINLINE float GetAO_Yaw() const { return Ao_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return Ao_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetViewCamera() const { return ViewCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsDead() const { return bIsDead; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE void SetHealth(float NewHealth) { Health = NewHealth; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	FORCEINLINE void SetShield(float NewShield) { Shield = NewShield; }
	FORCEINLINE UInputMappingContext* GetBlastCharacterMappingContext() const { return BlastCharacterMappingContext; }
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE UBuffComponent* GetBuffComponent() const { return BuffComponent; }
	FORCEINLINE bool GetDisableGameplayInput() const { return bDisableGameplayInput; }
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	bool IsLocallyReloading();


private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
		USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere , Category = Camera)
		UCameraComponent* ViewCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		 UWidgetComponent* OverheadWidget;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
		AWeapon* OverlappingWeapon;

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComponent;

	UPROPERTY(VisibleAnywhere)
	ULagCompensationComponent* LagCompensationComponent;

	UPROPERTY(VisibleAnywhere)
		UBuffComponent* BuffComponent;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();
	UFUNCTION(Server, Reliable)
		void ServerSwitchWeaponButtonPressed();

	float Ao_Yaw;
	float InterpAO_Yaw;
	float Ao_Pitch;
	FRotator StartingAimRotation;

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere , Category = Combat)
		UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* HitReactMontage;
	UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* DeathMontage;
	UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* ReloadMontage;
	UPROPERTY(EditAnywhere, Category = Combat)
		UAnimMontage* ThrowGrenadeMontage;

	UPROPERTY(EditAnywhere, Category = Camera)
		float CameraThreshold = 200.0f;

	bool bRotateRootBone;
	float TurnThreshold = 0.5f;
	FRotator ProxyRotationLastFrame;
	FRotator ProxyRotation;
	float ProxyYawDelta;
	float TimeSinceLastMovementReplication;
	float CalculateSpeed();

	void HideCharacter(bool bHide);

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.0f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.0f;
	UPROPERTY(EditAnywhere, Category = "Player Stats")
		float MaxShield = 100.0f;
	UPROPERTY(ReplicatedUsing = OnRep_Shield, EditAnywhere, Category = "Player Stats")
	float Shield = 0.0f;

	bool bIsDead = false;


	UFUNCTION()
		void OnRep_Health(float OldHealth);
	UFUNCTION()
		void OnRep_Shield(float OldShield);

	UPROPERTY()
	ABlasterPlayerController* BlasterPlayerController;

	FTimerHandle DeathTimerHandle;

	UPROPERTY(EditDefaultsOnly)
	float DeathDelay = 5.0f;


	void DeathTimerFinished();

	UPROPERTY()
	ABlasterPlayerState* BlasterPlayerState;


	void KillCam(float DeltaTime);

	UPROPERTY(ReplicatedUsing = OnRep_Killer)
	ABlasterCharacter* Killer;


	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* AttachedGrenade;

	UPROPERTY(EditAnywhere)
		TSubclassOf<AWeapon> DefaultWeaponClass;




};
