// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"


void AShotgun::Fire(const FVector& HitTarget)
{
	AWeapon::Fire(HitTarget);
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
		uint32 Hits = 0;
		TMap<ABlasterCharacter*, uint32> HitMap;
		for (uint32 i = 0; i < NumberOfPellets; i++)
		{
			FHitResult HitResult;
			WeaponTraceHit(HitResult, Start, HitTarget);

			ABlasterCharacter* Character = Cast<ABlasterCharacter>(HitResult.GetActor());
			if (Character && HasAuthority() && InstigatorController)
			{
				if (HitMap.Contains(Character))
				{
					HitMap[Character]++;
				}
				else
				{
					HitMap.Emplace(Character, 1);

				}
			}
			if (Character)
			{
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
