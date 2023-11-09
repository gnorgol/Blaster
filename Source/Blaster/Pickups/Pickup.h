// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"


class USphereComponent;
class USoundCue;
class UStaticMeshComponent;
class UNiagaraComponent;
class UNiagaraSystem;
UCLASS()
class BLASTER_API APickup : public AActor
{
	GENERATED_BODY()
		
public:	
	APickup();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY(EditAnywhere)
		USphereComponent* OverlapSphere;
	UPROPERTY(EditAnywhere)
		USoundCue* PickupSound;
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* PickupMesh;


protected:

	virtual void BeginPlay() override;


	UPROPERTY(EditAnywhere)
	float RotationRate = 50.0f;

	UPROPERTY(VisibleAnywhere)
		UNiagaraComponent* PickupEffectComponent;

	UPROPERTY(EditAnywhere)
		UNiagaraSystem* PickupEffect;

	FTimerHandle BindOverlapTimerHandle;
	float BindOverlapTimer = 0.25f;
	void BindOverlapFinished();

	

};
