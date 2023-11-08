
#include "PickupSpawnPoint.h"
#include "Pickup.h"

APickupSpawnPoint::APickupSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void APickupSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	StartSpawnPickupTimer((AActor*)nullptr);
	
}

void APickupSpawnPoint::SpawnPickup()
{
	int32 NumPickupsClasses = PickupClasses.Num();
	if (NumPickupsClasses > 0)
	{
		int32 SpawnIndex = FMath::RandRange(0, NumPickupsClasses - 1);
		SpawnedPickup = GetWorld()->SpawnActor<APickup>(PickupClasses[SpawnIndex], GetActorTransform());

		if (HasAuthority() && SpawnedPickup)
		{
			SpawnedPickup->OnDestroyed.AddDynamic(this, &APickupSpawnPoint::StartSpawnPickupTimer);
		}
	}

}

void APickupSpawnPoint::SpawnPickupTimerFinished()
{
	if (HasAuthority())
	{
		SpawnPickup();
	}
}

void APickupSpawnPoint::StartSpawnPickupTimer(AActor* DestroyedActor)
{
	const float SpawnPickupDelay = FMath::RandRange(SpawnPickupTimerMin, SpawnPickupTimerMax);
	GetWorldTimerManager().SetTimer(SpawnPickupTimerHandle, this, &APickupSpawnPoint::SpawnPickupTimerFinished, SpawnPickupDelay);
}

void APickupSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

