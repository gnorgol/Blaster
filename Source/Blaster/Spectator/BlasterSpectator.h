// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/SpectatorPawn.h"
#include "BlasterSpectator.generated.h"

/**
 * 
 */
class UCameraComponent;
class UFloatingPawnMovement;
class UInputMappingContext;
class UInputAction;
class UInputActionValue;
UCLASS()
class BLASTER_API ABlasterSpectator : public ASpectatorPawn
{
	GENERATED_BODY()
public:



	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* BlastCharacterMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* BlastCharacterMappingContextAZERTY;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* BlastCharacterMappingContextQWERTY;



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;




public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	const FString MoveForwardActionName = NSLOCTEXT("Blaster", "MoveForward", "Move Forward").ToString();
	const FString MoveRightActionName = NSLOCTEXT("Blaster", "MoveRight", "Move Right").ToString();
	const FString MoveLeftActionName = NSLOCTEXT("Blaster", "MoveLeft", "Move Left").ToString();
	const FString MoveBackwardActionName = NSLOCTEXT("Blaster", "MoveBackward", "Move Backwards").ToString();



};
