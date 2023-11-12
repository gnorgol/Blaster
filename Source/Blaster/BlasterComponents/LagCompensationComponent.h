

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LagCompensationComponent.generated.h"

class ABlasterPlayerController;
class ABlasterCharacter;
USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()
		UPROPERTY()
		FVector Location;
	UPROPERTY()
		FRotator Rotation;
	UPROPERTY()
		FVector Extent;
};
USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
		float Time;

	UPROPERTY()
		TMap<FName, FBoxInformation> HitBoxInfo;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULagCompensationComponent();
	friend ABlasterCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ShowFramePackage(FFramePackage& FramePackage,FColor Color);
protected:
	virtual void BeginPlay() override;
	void SaveFramePackage(FFramePackage& FramePackage);

private:
	UPROPERTY()
		ABlasterCharacter* Character;

	UPROPERTY()
		ABlasterPlayerController* Controller;
	

		
};
