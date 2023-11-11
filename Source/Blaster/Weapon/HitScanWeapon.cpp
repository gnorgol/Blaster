// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "WeaponTypes.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	bool bHitCharacter = false;
	if (Owner == nullptr)
	{
		return;
	}
	AController* InstigatorController = OwnerPawn->GetController();
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		FHitResult HitResult;
		WeaponTraceHit(HitResult, Start, HitTarget);

		ABlasterCharacter* Character = Cast<ABlasterCharacter>(HitResult.GetActor());
		if (Character && HasAuthority() && InstigatorController)
		{
			UGameplayStatics::ApplyDamage(Character, Damage, InstigatorController, this, UDamageType::StaticClass());
		}
		if (Character)
		{
			if (ImpactFleshParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFleshParticles, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
			}
			if (ImpactFleshSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, ImpactFleshSound, HitResult.ImpactPoint);
			}
		}
		else
		{
			if (ImpactParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
			}
			if (ImpactSurfaceSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, ImpactSurfaceSound, HitResult.ImpactPoint);
			}
		}
		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}
		if (FireSound)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}
	}
}

void AHitScanWeapon::WeaponTraceHit(FHitResult& OutHit, const FVector& TraceStart, const FVector& HitTraget)
{
	UWorld* World = GetWorld();
	if (World)
	{
		FVector TraceEnd = TraceStart + (HitTraget - TraceStart) * 1.25;
		World->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);
		FVector BeamEnd = TraceEnd;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
		}
		if (BeamParticle)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World, BeamParticle, TraceStart, FRotator::ZeroRotator, true);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}

		}
	}
}


