// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"



void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());
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
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector Start = SocketTransform.GetLocation();
		TMap<ABlasterCharacter*, uint32> HitMap;
		for (FVector_NetQuantize HitTarget : HitTargets) 
		{
			FHitResult HitResult;
			WeaponTraceHit(HitResult, Start, HitTarget);

			ABlasterCharacter* Character = Cast<ABlasterCharacter>(HitResult.GetActor());
			if (Character)
			{
				if (HitMap.Contains(Character))
				{
					HitMap[Character]++;
				}
				else
				{
					HitMap.Emplace(Character, 1);

				}
				if (ImpactFleshParticles)
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactFleshParticles, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
				}
				if (ImpactFleshSound)
				{
					UGameplayStatics::PlaySoundAtLocation(this, ImpactFleshSound, HitResult.ImpactPoint, 0.5f, FMath::RandRange(-0.5f, 0.5f));
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
					UGameplayStatics::PlaySoundAtLocation(this, ImpactSurfaceSound, HitResult.ImpactPoint, 0.5f, FMath::RandRange(-0.5f, 0.5f));
				}
			}



		}
		for (auto HitPair : HitMap)
		{
			if (InstigatorController && HasAuthority() && HitPair.Key)
			{
				UGameplayStatics::ApplyDamage(HitPair.Key, Damage * HitPair.Value, InstigatorController, this, UDamageType::StaticClass());
			}
		}
	}
}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName("MuzzleFlash");
	if (MuzzleFlashSocket == nullptr)
	{
		return ;
	}

	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();

	const FVector ToTarget = (HitTarget - TraceStart).GetSafeNormal();
	FVector SphereCenter = TraceStart + ToTarget * DistanceToSphere;


	for (uint32 i = 0; i < NumberOfPellets; i++)
	{
		const FVector RandimVector = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0, SphereRadius);
		const FVector EndLocation = SphereCenter + RandimVector;
		FVector ToEndLocation = EndLocation - TraceStart;
		ToEndLocation = TraceStart + ToEndLocation * TRACE_LENGTH / ToEndLocation.Size();
		HitTargets.Add(ToEndLocation);
	}

}
