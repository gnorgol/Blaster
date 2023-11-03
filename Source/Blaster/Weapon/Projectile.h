// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class UParticleSystemComponent;
class USoundCue;
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
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere)
		float Damage = 20.0f;
	UFUNCTION(BlueprintImplementableEvent)
		void CreateFieldsExplosionEffect(const FVector& Location);
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
		USoundCue* ImpactSound;
	bool bHitCharacter = false;

	UPROPERTY(VisibleAnywhere)
		UProjectileMovementComponent* ProjectileMovementComponent;


private:


	UPROPERTY(EditAnywhere)
		UParticleSystem* Tracer;

	UPROPERTY()
		UParticleSystemComponent* TracerComponent;


	

	UFUNCTION(NetMulticast, Reliable)
	void MulticastIsHitCharacter(AActor* OtherActor);

};
