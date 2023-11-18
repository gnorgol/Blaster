// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "WeaponTypes.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
class UParticleSystem;
UCLASS()
class BLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget, EWeaponType WeaponTypes) override;
	
protected:
	
	
	void WeaponTraceHit(FHitResult& OutHit,const FVector& TraceStart, const FVector& HitTraget );
	UPROPERTY(EditAnywhere)
		UParticleSystem* ImpactParticle;
	UPROPERTY(EditAnywhere)
		UParticleSystem* ImpactFleshParticles;
	UPROPERTY(EditAnywhere)
		UParticleSystem* BeamParticle;
	UPROPERTY(EditAnywhere)
		UParticleSystem* MuzzleFlash;
	UPROPERTY(EditAnywhere)
		USoundCue* FireSound;
	UPROPERTY(EditAnywhere)
		USoundCue* ImpactSurfaceSound;
	UPROPERTY(EditAnywhere)
		USoundCue* ImpactFleshSound;



};
