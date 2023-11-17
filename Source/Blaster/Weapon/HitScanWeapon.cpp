// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "WeaponTypes.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/BlasterComponents/LagCompensationComponent.h"
#include "WeaponTypes.h"

void AHitScanWeapon::Fire(const FVector& HitTarget, EWeaponType WeaponTypes)
{
	Super::Fire(HitTarget, WeaponTypes);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
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

		ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(HitResult.GetActor());
		if (BlasterCharacter && InstigatorController)
		{
			bool bCauseAuthorityDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
			if (HasAuthority() && bCauseAuthorityDamage)
			{
				const float DamageToApply = HitResult.BoneName.ToString() == FString("head") ? Damage * HeadShotMultiplier : Damage;
				UGameplayStatics::ApplyDamage(BlasterCharacter, DamageToApply, InstigatorController, this, UDamageType::StaticClass());
			}
			if (bUseServerSideRewind && !HasAuthority())
			{
				BlasterOwnerCharacter = BlasterOwnerCharacter == nullptr ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
				BlasterOwnerController = BlasterOwnerController == nullptr ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;
				if (BlasterOwnerController && BlasterOwnerCharacter && BlasterOwnerCharacter->GetLagCompensationComponent()&& BlasterOwnerCharacter->IsLocallyControlled())
				{
					BlasterOwnerCharacter->GetLagCompensationComponent()->ServerScoreRequest(BlasterCharacter, Start, HitTarget, BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime, this);
				}
			}
			
		}
		if (BlasterCharacter)
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
		else
		{
			OutHit.ImpactPoint = TraceEnd;
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


