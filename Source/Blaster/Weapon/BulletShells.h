// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletShells.generated.h"

class USoundCue;

UCLASS()
class BLASTER_API ABulletShells : public AActor
{
	GENERATED_BODY()
	
public:	

	ABulletShells();

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BulletShellsMesh;

	UPROPERTY(EditAnywhere)
	float ShellEjectImpulse;

	UPROPERTY(EditAnywhere)
	USoundCue* ShellEjectSound;

	int ShellBounceCount;

	UPROPERTY(EditAnywhere)
	int MaxShellBounceCount;

};
