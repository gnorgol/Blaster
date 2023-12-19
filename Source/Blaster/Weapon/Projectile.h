// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponTypes.h"
#include "Projectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class UParticleSystemComponent;
class USoundCue;
class UNiagaraComponent;
class UNiagaraSystem;


UCLASS()
class BLASTER_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	bool bUseServerSideRewind = false;
	FVector_NetQuantize TraceStart;
	FVector_NetQuantize100 InitialVelocity;

	UPROPERTY(EditAnywhere)
		float InitialSpeed = 15000.f;
	//Only for grenade and rocket
	UPROPERTY(EditAnywhere)
		float Damage = 20.0f;
	UPROPERTY(EditAnywhere)
		float HeadShotMultiplier = 2.0f;

	EWeaponType GetWeaponType() const { return WeaponType; }
	void SetWeaponType(EWeaponType Type) { WeaponType = Type; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void StartDestroyTimer();
	void DestroyTimerFinished();
	void ExplodeDamage();

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);


	UFUNCTION(BlueprintImplementableEvent)
		void CreateFieldsExplosionEffect(const FVector& Location);
	UFUNCTION(NetMulticast, Reliable)
		void MulticastCreateFieldsExplosionEffect(const FVector& Location);
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float MinDamage = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float DamageInnerRadius = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float DamageOuterRadius = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float DamageFalloff = 1.0f;

	UPROPERTY(EditAnywhere)
		UParticleSystem* ImpactParticles;
	UPROPERTY(EditAnywhere)
		UParticleSystem* ImpactFleshParticles;
	UPROPERTY(EditAnywhere)
		UBoxComponent* CollisionBox;
	UPROPERTY(EditAnywhere)
		USoundCue* ImpactSurfaceSound;
	UPROPERTY(EditAnywhere)
		USoundCue* ImpactFleshSound;
	bool bHitCharacter = false;

	UPROPERTY(VisibleAnywhere)
		UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(EditAnywhere, Category = "Effects")
		UNiagaraSystem* TrailSystem;

	
		UNiagaraComponent* TrailSystemComponent;

	void SpawnTrailSystem();

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* ProjectileMesh;
	UPROPERTY()
	EWeaponType WeaponType;
private:


	UPROPERTY(EditAnywhere)
		UParticleSystem* Tracer;

	UPROPERTY()
		UParticleSystemComponent* TracerComponent;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastIsHitCharacter(AActor* OtherActor);

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
		float DestroyDelay = 3.0f;

};
