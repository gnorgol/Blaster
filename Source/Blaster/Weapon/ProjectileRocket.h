// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"


/**
 * 
 */
class UNiagaraSystem;
class UNiagaraComponent;
class URocketMovementComponent;
UCLASS()
class BLASTER_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()

public:
	AProjectileRocket();


protected:
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                   FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void BeginPlay() override;
	void DestroyTimerFinished();
	virtual void Destroyed() override;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* TrailSystem;

	UPROPERTY()
	UNiagaraComponent* TrailSystemComponent;

	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundCue* ProjectileLoop;

	UPROPERTY()
	UAudioComponent* ProjectileLoopComponent;

	UPROPERTY(EditAnywhere, Category = "Effects")
	USoundAttenuation* LoopingSoundAttenuation;

	UPROPERTY(VisibleAnywhere)
	URocketMovementComponent* RocketMovementComponent;
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere)
	float DestroyDelay = 3.0f;
	
};
