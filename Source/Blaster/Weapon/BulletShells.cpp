// Fill out your copyright notice in the Description page of Project Settings.


#include "BulletShells.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


// Sets default values
ABulletShells::ABulletShells()
{

	PrimaryActorTick.bCanEverTick = false;

	BulletShellsMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletShellsMesh"));
	SetRootComponent(BulletShellsMesh);
	BulletShellsMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	BulletShellsMesh->SetSimulatePhysics(true);
	BulletShellsMesh->SetEnableGravity(true);
	BulletShellsMesh->SetNotifyRigidBodyCollision(true);
	ShellEjectImpulse = 10;
	MaxShellBounceCount = 2;

}


void ABulletShells::BeginPlay()
{
	Super::BeginPlay();

	BulletShellsMesh->AddImpulse(GetActorForwardVector() * ShellEjectImpulse);
	BulletShellsMesh->OnComponentHit.AddDynamic(this, &ABulletShells::OnHit);
	SetLifeSpan(5);	
}

void ABulletShells::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ShellEjectSound && ShellBounceCount < MaxShellBounceCount)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellEjectSound, GetActorLocation());
		ShellBounceCount++;
	}
}



