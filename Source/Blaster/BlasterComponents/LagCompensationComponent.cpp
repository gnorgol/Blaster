#include "LagCompensationComponent.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"	
#include "Blaster/Weapon/Weapon.h"
#include "Blaster/Blaster.h"



ULagCompensationComponent::ULagCompensationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}






void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();
}



FFramePackage ULagCompensationComponent::InterpolateBetweenFrames(const FFramePackage& OlderFrame, const FFramePackage& YoungerFrame, float HitTime)
{
	const float Distance = YoungerFrame.Time - OlderFrame.Time;
	const float Alpha = FMath::Clamp((HitTime - OlderFrame.Time) / Distance, 0.f, 1.f);
	FFramePackage InterpolatedFrame;
	InterpolatedFrame.Time = HitTime;
	for (auto& YougerPair : YoungerFrame.HitBoxInfo)
	{
		const FName& BoxInfoName = YougerPair.Key;

		const FBoxInformation& OlderBoxInfo = OlderFrame.HitBoxInfo[BoxInfoName];
		const FBoxInformation& YoungerBoxInfo = YoungerFrame.HitBoxInfo[BoxInfoName];

		FBoxInformation InterpolatedBoxInfo;

		InterpolatedBoxInfo.Location = FMath::VInterpTo(OlderBoxInfo.Location, YoungerBoxInfo.Location, 1.f, Alpha);
		InterpolatedBoxInfo.Rotation = FMath::RInterpTo(OlderBoxInfo.Rotation, YoungerBoxInfo.Rotation, 1.f, Alpha);

		InterpolatedBoxInfo.Extent = YoungerBoxInfo.Extent;

		InterpolatedFrame.HitBoxInfo.Add(BoxInfoName, InterpolatedBoxInfo);

	}
	return InterpolatedFrame;
}

FServerSideRewindResult ULagCompensationComponent::ConfirmHit(const FFramePackage& Package, ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation)
{
	if (HitCharacter ==nullptr)
	{
		return FServerSideRewindResult();
	}
	FFramePackage CurrentFrame;
	CacheBoxPositions(HitCharacter, CurrentFrame);
	MoveBoxes(HitCharacter, Package);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);

	// Enable collision for the head box
	UBoxComponent* HeadBox = HitCharacter->HitCollisionBoxes[FName("head")];
	HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeadBox->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);

	FHitResult ConfirmHitResult;
	const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
	UWorld* World = GetWorld();
	if (World)
	{
		World->LineTraceSingleByChannel(ConfirmHitResult, TraceStart, TraceEnd, ECC_HitBox);
		if (ConfirmHitResult.bBlockingHit)
		{
			if (ConfirmHitResult.Component.IsValid())
			{
				UBoxComponent* HitBox = Cast<UBoxComponent>(ConfirmHitResult.Component.Get());
				if (HitBox)
				{
					DrawDebugBox(GetWorld(), HitBox->GetComponentLocation(), HitBox->GetScaledBoxExtent(), FQuat(HitBox->GetComponentRotation()), FColor::Red, false, 8.f);
				}

			}
			//hit the head
			ResetHitBoxes(HitCharacter, CurrentFrame);
			EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
			return FServerSideRewindResult(true, true);
		}
		else
		{
			// Check the rest of the body
			for (auto& BoxPair : HitCharacter->HitCollisionBoxes)
			{
				if (BoxPair.Value != nullptr)
				{
					BoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					BoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
				}
			}
			World->LineTraceSingleByChannel(ConfirmHitResult, TraceStart, TraceEnd, ECC_HitBox);
			if (ConfirmHitResult.bBlockingHit)
			{
				if (ConfirmHitResult.Component.IsValid())
				{
					UBoxComponent* HitBox = Cast<UBoxComponent>(ConfirmHitResult.Component.Get());
					if (HitBox)
					{
						DrawDebugBox(GetWorld(), HitBox->GetComponentLocation(), HitBox->GetScaledBoxExtent(), FQuat(HitBox->GetComponentRotation()), FColor::Blue, false, 8.f);
					}

				}
				//hit the body
				ResetHitBoxes(HitCharacter, CurrentFrame);
				EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
				return FServerSideRewindResult(true, false);
			}
		}
	}
	//missed hit
	ResetHitBoxes(HitCharacter, CurrentFrame);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
	return FServerSideRewindResult(false, false);


}
FServerSideRewindResult ULagCompensationComponent::ProjectileConfirmHit(const FFramePackage& Package, ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
{
	FFramePackage CurrentFrame;
	CacheBoxPositions(HitCharacter, CurrentFrame);
	MoveBoxes(HitCharacter, Package);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::NoCollision);

	// Enable collision for the head box
	UBoxComponent* HeadBox = HitCharacter->HitCollisionBoxes[FName("head")];
	HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HeadBox->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);

	FPredictProjectilePathParams PathParams;
	PathParams.bTraceWithCollision = true;
	PathParams.MaxSimTime = MaxRecordTime;
	PathParams.LaunchVelocity = InitialVelocity;
	PathParams.StartLocation = TraceStart;
	PathParams.SimFrequency = 15.f;
	PathParams.ProjectileRadius = 5.f;
	PathParams.TraceChannel = ECC_HitBox;
	PathParams.ActorsToIgnore.Add(GetOwner());


	FPredictProjectilePathResult PathResult;
	UGameplayStatics::PredictProjectilePath(GetWorld(), PathParams, PathResult);

	if (PathResult.HitResult.bBlockingHit)
	{
		if (PathResult.HitResult.Component.IsValid())
		{
			UBoxComponent* HitBox = Cast<UBoxComponent>(PathResult.HitResult.Component.Get());
			if (HitBox)
			{
				DrawDebugBox(GetWorld(), HitBox->GetComponentLocation(), HitBox->GetScaledBoxExtent(), FQuat(HitBox->GetComponentRotation()), FColor::Red, false, 8.f);
			}

		}
		//hit the head
		ResetHitBoxes(HitCharacter, CurrentFrame);
		EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
		return FServerSideRewindResult(true, true);
	}
	else
	{
		// Check the rest of the body
		for (auto& BoxPair : HitCharacter->HitCollisionBoxes)
		{
			if (BoxPair.Value != nullptr)
			{
				BoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				BoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			}
		}
		UGameplayStatics::PredictProjectilePath(GetWorld(), PathParams, PathResult);
		if (PathResult.HitResult.bBlockingHit)
		{
			if (PathResult.HitResult.Component.IsValid())
			{
				UBoxComponent* HitBox = Cast<UBoxComponent>(PathResult.HitResult.Component.Get());
				if (HitBox)
				{
					DrawDebugBox(GetWorld(), HitBox->GetComponentLocation(), HitBox->GetScaledBoxExtent(), FQuat(HitBox->GetComponentRotation()), FColor::Blue, false, 8.f);
				}

			}
			//hit the body
			ResetHitBoxes(HitCharacter, CurrentFrame);
			EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
			return FServerSideRewindResult(true, false);
		}
	}
	//missed hit
	ResetHitBoxes(HitCharacter, CurrentFrame);
	EnableCharacterMeshCollision(HitCharacter, ECollisionEnabled::QueryAndPhysics);
	return FServerSideRewindResult(false, false);
	
}
FShotgunServerSideRewindResult ULagCompensationComponent::ConfirmShotgunHit(const TArray<FFramePackage>& FramePackages, const FVector_NetQuantize TraceStart, const TArray<FVector_NetQuantize>& HitLocations)
{
	for (auto& Frame : FramePackages)
	{
		if (Frame.Character == nullptr)
		{
			return FShotgunServerSideRewindResult();
		}
	}

	FShotgunServerSideRewindResult ShotgunResult;
	TArray<FFramePackage> CurrentFrames;
	for (auto& Frame : FramePackages)
	{
		FFramePackage CurrentFrame;
		CurrentFrame.Character = Frame.Character;
		CacheBoxPositions(Frame.Character, CurrentFrame);
		MoveBoxes(Frame.Character, Frame);
		EnableCharacterMeshCollision(Frame.Character, ECollisionEnabled::NoCollision);
		CurrentFrames.Add(CurrentFrame);

	}
	for (auto& Frame : FramePackages)
	{
		// Enable collision for the head box
		UBoxComponent* HeadBox = Frame.Character->HitCollisionBoxes[FName("head")];
		HeadBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		HeadBox->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
	}
	UWorld* World = GetWorld();
	//Check for headshots
	for (auto& HitLocation : HitLocations)
	{
		FHitResult ConfirmHitResult;
		const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;
		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, true);
		if (World)
		{
			World->LineTraceSingleByChannel(
				ConfirmHitResult,
				TraceStart,
				TraceEnd,
				ECC_HitBox
			);
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(ConfirmHitResult.GetActor());
			if (BlasterCharacter)
			{
				if (ConfirmHitResult.Component.IsValid())
				{
					UBoxComponent* HitBox = Cast<UBoxComponent>(ConfirmHitResult.Component.Get());
					if (HitBox)
					{
						DrawDebugBox(GetWorld(), HitBox->GetComponentLocation(), HitBox->GetScaledBoxExtent(), FQuat(HitBox->GetComponentRotation()), FColor::Red, false, 8.f);
					}

				}
				if (ShotgunResult.HeadShots.Contains(BlasterCharacter))
				{
					ShotgunResult.HeadShots[BlasterCharacter]++;
				}
				else
				{
					ShotgunResult.HeadShots.Emplace(BlasterCharacter, 1);
				}
			}

		}
	}

	// Enable collision for all boxes except the head
	for (auto& Frame : FramePackages)
	{
		for (auto& BoxPair : Frame.Character->HitCollisionBoxes)
		{
			if (BoxPair.Value != nullptr)
			{
				BoxPair.Value->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				BoxPair.Value->SetCollisionResponseToChannel(ECC_HitBox, ECollisionResponse::ECR_Block);
			}
		}
		UBoxComponent* HeadBox = Frame.Character->HitCollisionBoxes[FName("head")];
		HeadBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	}
	//Check for body shots
	for (auto& HitLocation : HitLocations)
	{
		FHitResult ConfirmHitResult;
		const FVector TraceEnd = TraceStart + (HitLocation - TraceStart) * 1.25f;

		if (World)
		{
			World->LineTraceSingleByChannel(ConfirmHitResult, TraceStart, TraceEnd, ECC_HitBox);
			ABlasterCharacter* HitCharacter = Cast<ABlasterCharacter>(ConfirmHitResult.GetActor());

			if (HitCharacter)
			{
				if (ConfirmHitResult.Component.IsValid())
				{
					UBoxComponent* HitBox = Cast<UBoxComponent>(ConfirmHitResult.Component.Get());
					if (HitBox)
					{
						DrawDebugBox(GetWorld(), HitBox->GetComponentLocation(), HitBox->GetScaledBoxExtent(), FQuat(HitBox->GetComponentRotation()), FColor::Blue, false, 8.f);
					}

				}
				if (ShotgunResult.BodyShots.Contains(HitCharacter))
				{
					ShotgunResult.BodyShots[HitCharacter]++;
				}
				else
				{
					ShotgunResult.BodyShots.Emplace(HitCharacter, 1);	
				}
			}

		}

	}
	// Reset all boxes
	for (auto& Frame : CurrentFrames)
	{
		ResetHitBoxes(Frame.Character, Frame);
		EnableCharacterMeshCollision(Frame.Character, ECollisionEnabled::QueryAndPhysics);
	}
	return ShotgunResult;
}

void ULagCompensationComponent::CacheBoxPositions(ABlasterCharacter* HitCharacter, FFramePackage& OutFramePackage)
{
	if (HitCharacter == nullptr)
	{
		return;
	}
	for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		if (HitBoxPair.Value != nullptr)
		{
			FBoxInformation BoxInformation;
			BoxInformation.Location = HitBoxPair.Value->GetComponentLocation();
			BoxInformation.Rotation = HitBoxPair.Value->GetComponentRotation();
			BoxInformation.Extent = HitBoxPair.Value->GetScaledBoxExtent();
			OutFramePackage.HitBoxInfo.Add(HitBoxPair.Key, BoxInformation);

		}

	}
}

void ULagCompensationComponent::MoveBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& Package)
{
	if (HitCharacter == nullptr)
	{
		return;
	}
	for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		if (HitBoxPair.Value != nullptr)
		{
			HitBoxPair.Value->SetWorldLocation(Package.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(Package.HitBoxInfo[HitBoxPair.Key].Rotation);
			HitBoxPair.Value->SetBoxExtent(Package.HitBoxInfo[HitBoxPair.Key].Extent);
		}
	}
}

void ULagCompensationComponent::ResetHitBoxes(ABlasterCharacter* HitCharacter, const FFramePackage& Package)
{
	if (HitCharacter == nullptr)
	{
		return;
	}
	for (auto& HitBoxPair : HitCharacter->HitCollisionBoxes)
	{
		if (HitBoxPair.Value != nullptr)
		{
			HitBoxPair.Value->SetWorldLocation(Package.HitBoxInfo[HitBoxPair.Key].Location);
			HitBoxPair.Value->SetWorldRotation(Package.HitBoxInfo[HitBoxPair.Key].Rotation);
			HitBoxPair.Value->SetBoxExtent(Package.HitBoxInfo[HitBoxPair.Key].Extent);
			HitBoxPair.Value->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ULagCompensationComponent::EnableCharacterMeshCollision(ABlasterCharacter* HitCharacter, ECollisionEnabled::Type CollisionType)
{
	if (HitCharacter && HitCharacter->GetMesh())
	{
		HitCharacter->GetMesh()->SetCollisionEnabled(CollisionType);
	}
	
}

void ULagCompensationComponent::SaveFramePackage()
{
	if (Character == nullptr || !Character->HasAuthority())
	{
		return;
	}
	if (FrameHistory.Num() <= 1)
	{
		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);
	}
	else
	{
		float HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		while (HistoryLength > MaxRecordTime)
		{
			FrameHistory.RemoveNode(FrameHistory.GetTail());
			HistoryLength = FrameHistory.GetHead()->GetValue().Time - FrameHistory.GetTail()->GetValue().Time;
		}
		FFramePackage ThisFrame;
		SaveFramePackage(ThisFrame);
		FrameHistory.AddHead(ThisFrame);

		//ShowFramePackage(FrameHistory.GetHead()->GetValue(), FColor::Red);
	}
}

void ULagCompensationComponent::SaveFramePackage(FFramePackage& FramePackage)
{
	Character = Character == nullptr ? Cast<ABlasterCharacter>(GetOwner()) : Character;
	if (Character)
	{
		FramePackage.Time = GetWorld()->GetTimeSeconds();
		FramePackage.Character = Character;
		for (auto& BoxPair : Character->HitCollisionBoxes)
		{
			FBoxInformation BoxInformation;
			BoxInformation.Location = BoxPair.Value->GetComponentLocation();
			BoxInformation.Rotation = BoxPair.Value->GetComponentRotation();
			BoxInformation.Extent = BoxPair.Value->GetScaledBoxExtent();
			FramePackage.HitBoxInfo.Add(BoxPair.Key, BoxInformation);
		}
	}
}



void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SaveFramePackage();

}

FServerSideRewindResult ULagCompensationComponent::ServerSideRewind(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime)
{
	FFramePackage FrameToCheck = GetFrameToCheck(HitCharacter, HitTime);
	return ConfirmHit(FrameToCheck, HitCharacter, TraceStart, HitLocation);
}
FServerSideRewindResult ULagCompensationComponent::ProjectileServerSideRewind(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
{
	FFramePackage FrameToCheck = GetFrameToCheck(HitCharacter, HitTime);
	return ProjectileConfirmHit(FrameToCheck, HitCharacter, TraceStart, InitialVelocity, HitTime);
}

FShotgunServerSideRewindResult ULagCompensationComponent::ShotgunServerSideRewind(const TArray<ABlasterCharacter*>& HitCharacters, const FVector_NetQuantize TraceStart, const TArray<FVector_NetQuantize>& HitLocations, float HitTime)
{
	TArray<FFramePackage> FramesToCheck;
	for (ABlasterCharacter* HitCharacter : HitCharacters)
	{
		FramesToCheck.Add(GetFrameToCheck(HitCharacter, HitTime));
	}
	return ConfirmShotgunHit(FramesToCheck, TraceStart, HitLocations);
}



FFramePackage ULagCompensationComponent::GetFrameToCheck(ABlasterCharacter* HitCharacter, float HitTime)
{
	bool bReturn =
		HitCharacter == nullptr ||
		HitCharacter->GetLagCompensationComponent() == nullptr ||
		HitCharacter->GetLagCompensationComponent()->FrameHistory.GetHead() == nullptr ||
		HitCharacter->GetLagCompensationComponent()->FrameHistory.GetTail() == nullptr;
	if (bReturn)
	{
		return FFramePackage();
	}
	FFramePackage FrameToCheck;
	bool bShouldInterpolate = true;
	// Frame History of the hit character
	const TDoubleLinkedList<FFramePackage>& History = HitCharacter->GetLagCompensationComponent()->FrameHistory;
	const float OldestHistoryTime = History.GetTail()->GetValue().Time;
	const float NewestHistoryTime = History.GetHead()->GetValue().Time;
	if (OldestHistoryTime > HitTime)
	{
		// too laggy for a server Rewind
		return FFramePackage();
	}
	if (OldestHistoryTime == HitTime)
	{
		FrameToCheck = History.GetTail()->GetValue();
		bShouldInterpolate = false;
	}
	if (NewestHistoryTime <= HitTime)
	{
		FrameToCheck = History.GetHead()->GetValue();
		bShouldInterpolate = false;
	}
	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* YoungerNode = History.GetHead();
	TDoubleLinkedList<FFramePackage>::TDoubleLinkedListNode* OlderNode = YoungerNode;
	while (OlderNode->GetValue().Time > HitTime)
	{
		if (OlderNode->GetNextNode() == nullptr)
		{
			break;
		}
		OlderNode = OlderNode->GetNextNode();
		if (OlderNode->GetValue().Time > HitTime)
		{
			YoungerNode = OlderNode;
		}
	}
	if (OlderNode->GetValue().Time == HitTime)
	{
		FrameToCheck = OlderNode->GetValue();
		bShouldInterpolate = false;
	}
	if (bShouldInterpolate)
	{
		// Interpolate between Younger and Older Node
		FrameToCheck = InterpolateBetweenFrames(OlderNode->GetValue(), YoungerNode->GetValue(), HitTime);


	}
	FrameToCheck.Character = HitCharacter;
	return FrameToCheck;
}



void ULagCompensationComponent::ServerScoreRequest_Implementation(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize& HitLocation, float HitTime, AWeapon* DamageCauser)
{
	FServerSideRewindResult Result = ServerSideRewind(HitCharacter, TraceStart, HitLocation, HitTime);

	if (HitCharacter && DamageCauser && Result.bHitConfirmed)
	{
		UGameplayStatics::ApplyDamage(HitCharacter, DamageCauser->GetDamage(), Character->Controller, DamageCauser, UDamageType::StaticClass());
	}
}
void ULagCompensationComponent::ServerProjectileScoreRequest_Implementation(ABlasterCharacter* HitCharacter, const FVector_NetQuantize& TraceStart, const FVector_NetQuantize100& InitialVelocity, float HitTime)
{
	FServerSideRewindResult Result = ProjectileServerSideRewind(HitCharacter, TraceStart, InitialVelocity, HitTime);

	if (HitCharacter && Character && Result.bHitConfirmed && Character->GetEquippedWeapon())
	{
		UGameplayStatics::ApplyDamage(HitCharacter, Character->GetEquippedWeapon()->GetDamage(), Character->Controller, Character->GetEquippedWeapon(), UDamageType::StaticClass());
	}
}
void ULagCompensationComponent::ServerShotgunScoreRequest_Implementation(const TArray<ABlasterCharacter*>& HitCharacters, const FVector_NetQuantize& TraceStart, const TArray<FVector_NetQuantize>& HitLocations, float HitTime)
{
	FShotgunServerSideRewindResult Result = ShotgunServerSideRewind(HitCharacters, TraceStart, HitLocations, HitTime);
	for (auto& HitCharacter : HitCharacters)
	{
		if (Character == nullptr || HitCharacter == nullptr || HitCharacter->GetEquippedWeapon() == nullptr)
		{
			continue;
		}
		float TotalDamage = 0.f;
		if (Result.HeadShots.Contains(HitCharacter))
		{
			float HeadShotDamage = Result.HeadShots[HitCharacter] * HitCharacter->GetEquippedWeapon()->GetDamage();
			TotalDamage += HeadShotDamage;
		}
		if (Result.BodyShots.Contains(HitCharacter))
		{
			float BodyShotDamage = Result.BodyShots[HitCharacter] * HitCharacter->GetEquippedWeapon()->GetDamage();
			TotalDamage += BodyShotDamage;
		}
		UGameplayStatics::ApplyDamage(HitCharacter, TotalDamage, Character->Controller, HitCharacter->GetEquippedWeapon(), UDamageType::StaticClass());
		
	}
}

void ULagCompensationComponent::ShowFramePackage(const FFramePackage& Package, const FColor& Color)
{
	for (auto& BoxPair : Package.HitBoxInfo)
	{
		DrawDebugBox(GetWorld(), BoxPair.Value.Location, BoxPair.Value.Extent, BoxPair.Value.Rotation.Quaternion(), Color, false, MaxRecordTime);
	}
}

