// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "RocketMovementComponent.h"
#include "NiagaraComponent.h"


AProjectileRocket::AProjectileRocket()
{
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rocket Mesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RocketMovementComponent = CreateDefaultSubobject<URocketMovementComponent>(TEXT("RocketMovementComponent"));
	RocketMovementComponent->bRotationFollowsVelocity = true;
	RocketMovementComponent->SetIsReplicated(true);

}

void AProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == GetOwner())
	{
		return;
	}
	ExplodeDamage();

	StartDestroyTimer();


	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
	}
	if (ImpactSurfaceSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSurfaceSound, GetActorLocation());
	}
	if (ProjectileMesh)
	{
		ProjectileMesh->SetVisibility(false);
	}
	if (CollisionBox)
	{
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (TrailSystemComponent)
	{
		TrailSystemComponent->Deactivate();
	}
	if (ProjectileLoopComponent && ProjectileLoopComponent->IsPlaying())
	{
		ProjectileLoopComponent->Stop();
	}

}

void AProjectileRocket::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority())
	{
		CollisionBox->OnComponentHit.AddDynamic(this, &AProjectileRocket::OnHit);
	}

	SpawnTrailSystem();
	if (ProjectileLoop && LoopingSoundAttenuation)
	{
		ProjectileLoopComponent = UGameplayStatics::SpawnSoundAttached(ProjectileLoop, GetRootComponent(), FName(), GetActorLocation(), EAttachLocation::KeepWorldPosition, false, 1.0f, 1.0f, 0.0f, LoopingSoundAttenuation, (USoundConcurrency*)nullptr, false);
	}


}



void AProjectileRocket::Destroyed()
{
}
