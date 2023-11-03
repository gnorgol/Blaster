// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Kismet/GameplayStatics.h"

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
	if (MuzzleFlashSocket && InstigatorController)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();
		FVector End = Start + (HitTarget - Start) * 1.25;
		
		FHitResult HitResult;
		UWorld* World = GetWorld();
		if (World)
		{
			World->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility);
				if (HitResult.bBlockingHit)
				{
					ABlasterCharacter* Character = Cast<ABlasterCharacter>(HitResult.GetActor());
					if (Character)
					{
						if (HasAuthority())
						{
							UGameplayStatics::ApplyDamage(Character, Damage, InstigatorController, this, UDamageType::StaticClass());
						}
						if (ImpactFleshParticles)
						{
							UGameplayStatics::SpawnEmitterAtLocation(World, ImpactFleshParticles, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
						}
					}
					else
					{
						if (ImpactParticle)
						{
							UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticle, HitResult.ImpactPoint, HitResult.ImpactNormal.Rotation());
						}
					}
				}
		}

	}
}
