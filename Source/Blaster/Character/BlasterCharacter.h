// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Blaster/Interfaces/InteractWithCrosshairsInterface.h"
#include "BlasterCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AWeapon;
class UCombatComponent;
class UAnimMontage;
class ABlasterPlayerController;
class ABlasterPlayerState;
class UWidgetComponent;

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

	UFUNCTION(NetMulticast, Reliable)
	void MulticastEliminated();

	void RagdollDeath();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRagdollDeath();

	void PlayHitReactMontage();
	void PlayDeathMontage();
	void PlayReloadMontage();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

	void Move(const FInputActionValue& Value);
	virtual void Jump() override;
	void Look(const FInputActionValue& Value);
	void Equip(const FInputActionValue& Value);
	void CrouchPressed(const FInputActionValue& Value);
	void AimPressed(const FInputActionValue& Value);
	void ReloadPressed(const FInputActionValue& Value);
	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	void FirePressed(const FInputActionValue& Value);

	UFUNCTION()
	void ReceiveDamage(AActor* DamageActor, float DamageAmount, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
	void UpdateHUDHealth();



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

	void HideCameraIfCharacterCloseToWall();
	// Poll for any classes and initialize HUD
	void PollInit();

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
	FORCEINLINE UInputMappingContext* GetBlastCharacterMappingContext() const { return BlastCharacterMappingContext; }



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

	UPROPERTY(VisibleAnywhere)
	UCombatComponent* CombatComponent;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

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

	bool bIsDead = false;


	UFUNCTION()
	void OnRep_Health();
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





};
