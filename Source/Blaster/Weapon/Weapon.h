// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial State"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_EquippedSecondary UMETA(DisplayName = "Equipped Secondary"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),

	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};
UENUM(BlueprintType)
enum class EFireType : uint8
{
	EFT_HitScan UMETA(DisplayName = "Hit Scan Weapon"),
	EFT_Projectile UMETA(DisplayName = "Projectile Weapon"),
	EFT_Shotgun UMETA(DisplayName = "Shotgun Weapon"),

	EFT_MAX UMETA(DisplayName = "DefaultMAX")
};


class USphereComponent;
class UWidgetComponent;
class UAnimationAsset;
class UTexture2D;
class ABlasterCharacter;
class ABlasterPlayerController;
class USoundCue;


UCLASS()
class BLASTER_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;
	void ShowPickupWidget(bool bShowWidget);
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; }
	FORCEINLINE float GetZoomedFOV() const { return ZoomedFOV; }
	FORCEINLINE float GetZoomInterpSpeed() const { return ZoomInterpSpeed; }
	FORCEINLINE float GetUnZoomedInterpSpeed() const { return UnZoomedInterpSpeed; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE int32 GetCurrentAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagCapacity() const { return MagCapacity; }
	virtual void Fire(const FVector& HitTarget);
	void DropWeapon();
	void AddAmmo(int32 AmmoToAdd);

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsCenter;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsLeft;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsRight;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsTop;
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		UTexture2D* CrosshairsBottom;

	UPROPERTY(EditAnywhere, Category = "Combat")
		float FireDelay = .15f;

	UPROPERTY(EditAnywhere, Category = "Combat")
		bool bAutomaticFire = true;
	void SetHUDAmmo();
	bool IsEmpty();
	bool IsFull();

	UPROPERTY(EditAnywhere, Category = "Combat")
		USoundCue* EquipSound;

	void EnableCustomDepth(bool bEnable);

	bool bDestroyWeapon = false;
	UPROPERTY(EditAnywhere)
		EFireType FireType;

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
		bool bUseScatter = false;

	FVector TraceEndWithScatter(const FVector& TraceEnd) const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnWeaponStateSet() ;
	virtual void OnEquipped();
	virtual void OnDropped();
	virtual void OnEquippedSecondary();

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
		float DistanceToSphere = 800.0f;
	UPROPERTY(EditAnywhere, Category = "Weapon Scatter")
		float SphereRadius = 75.f;
private:
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UWidgetComponent* PickupWidget;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TSubclassOf<class ABulletShells> BulletShellsClass;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		float ZoomedFOV = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		float ZoomInterpSpeed = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		float UnZoomedInterpSpeed = 20.0f;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Ammo, Category = "Weapon Properties")
	int32 Ammo;

	UFUNCTION()
	void OnRep_Ammo();

	void SpendRound();



	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
		int32 MagCapacity;

	UPROPERTY()
	ABlasterCharacter* BlasterOwnerCharacter;
	UPROPERTY()
	ABlasterPlayerController* BlasterOwnerController;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EWeaponType WeaponType;

};