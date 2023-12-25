// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterSpectator.h"

#include "GameFramework/FloatingPawnMovement.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "InputMappingContext.h"
#include <Blaster/PlayerController/SaveInputMapping.h>
#include <Kismet/GameplayStatics.h>
#include "PlayerMappableKeySettings.h"
#include "GameFramework/InputSettings.h"
#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOMINMAX
#include <windows.h>






void ABlasterSpectator::BeginPlay()
{
	Super::BeginPlay();

}


void ABlasterSpectator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABlasterSpectator::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{

	USaveInputMapping* SaveGameInstance = Cast<USaveInputMapping>(UGameplayStatics::LoadGameFromSlot(TEXT("BlasterInputMapping"), 0));
	if (SaveGameInstance && !SaveGameInstance->EnhancedActionMappings.IsEmpty())
	{
		//Load the saved mapping and add it to the subsystem
		BlastCharacterMappingContext->UnmapAll();
		BlastCharacterMappingContext->Mappings = SaveGameInstance->EnhancedActionMappings;
	}
	else
	{
		//Detect if the keyborad is an AZERTY keyboard or a QWERTY keyboard
		switch (PRIMARYLANGID(HIWORD(GetKeyboardLayout(0))))
		{
		case LANG_FRENCH:
			BlastCharacterMappingContext->UnmapAll();
			BlastCharacterMappingContext->Mappings = BlastCharacterMappingContextAZERTY->GetMappings();
			break;
		case LANG_ENGLISH:
			BlastCharacterMappingContext->UnmapAll();
			BlastCharacterMappingContext->Mappings = BlastCharacterMappingContextQWERTY->GetMappings();
			break;
		default:
			BlastCharacterMappingContext->UnmapAll();
			BlastCharacterMappingContext->Mappings = BlastCharacterMappingContextQWERTY->GetMappings();
			break;
		}
	}
	FInputAxisKeyMapping MoveForwardMapping;
	FInputAxisKeyMapping MoveBackwardsMapping;
	FInputAxisKeyMapping MoveLeftMapping;
	FInputAxisKeyMapping MoveRightMapping;

	for (const FEnhancedActionKeyMapping Mapping : BlastCharacterMappingContext->GetMappings()) {
		if (Mapping.GetPlayerMappableKeySettings())
		{
			FString ActionName = Mapping.GetPlayerMappableKeySettings()->DisplayName.ToString();
			if (ActionName == "Move Forward")
			{
				MoveForwardMapping.AxisName = "MoveForward";
				MoveForwardMapping.Key = Mapping.Key;
				MoveForwardMapping.Scale = 1;
			}
			else if (ActionName == "Move Backwards")
			{
				MoveBackwardsMapping.AxisName = "MoveForward";
				MoveBackwardsMapping.Key = Mapping.Key;
				MoveBackwardsMapping.Scale = -1;
			}
			else if (ActionName == "Move Left")
			{
				MoveLeftMapping.AxisName = "MoveRight";
				MoveLeftMapping.Key = Mapping.Key;
				MoveLeftMapping.Scale = -1;
			}
			else if (ActionName == "Move Right")
			{
				MoveRightMapping.AxisName = "MoveRight";
				MoveRightMapping.Key = Mapping.Key;
				MoveRightMapping.Scale = 1;
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Mapping.GetPlayerMappableKeySettings() is null"));
		}

	}

	UInputSettings* InputSettings = const_cast<UInputSettings*>(GetDefault<UInputSettings>());

	//Remove the old mappings
	for (int32 i = 0; i < InputSettings->GetAxisMappings().Num(); i++)
	{
		FInputAxisKeyMapping Mapping = InputSettings->GetAxisMappings()[i];
		FString ActionName = Mapping.AxisName.ToString();
		if (ActionName == "MoveForward" || ActionName == "MoveRight")
		{
			InputSettings->RemoveAxisMapping(Mapping);
			i--;
		}
	}

	InputSettings->AddAxisMapping(MoveForwardMapping);
	InputSettings->AddAxisMapping(MoveBackwardsMapping);
	InputSettings->AddAxisMapping(MoveLeftMapping);
	InputSettings->AddAxisMapping(MoveRightMapping);
	InputSettings->SaveConfig();
	InputSettings->ForceRebuildKeymaps();


	PlayerInputComponent->BindAxis("MoveForward", this, &ABlasterSpectator::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABlasterSpectator::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ABlasterSpectator::LookUpAtRate);
	PlayerInputComponent->BindAxis("LookRight", this, &ABlasterSpectator::TurnAtRate);


}
