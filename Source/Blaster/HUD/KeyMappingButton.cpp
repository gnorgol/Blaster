// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyMappingButton.h"
#include "Components/InputKeySelector.h"
#include "InputModifiers.h"
#include "InputMappingContext.h"
#include "Components/InputKeySelector.h"
#include "Framework/Commands/InputChord.h"
#include <Blaster/Character/BlasterCharacter.h>
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"


void UKeyMappingButton::SetKey(FInputChord SelectedKey)
{
	ABlasterCharacter *BlastCharacter = Cast<ABlasterCharacter>(GetOwningPlayerPawn());
	
	if (BlastCharacter)
	{
		UInputMappingContext* MappingContext = BlastCharacter->BlastCharacterMappingContext;
		TArray<TObjectPtr<UInputModifier>> KeyModifier;
		KeyModifier = MappingContext->Mappings[KeyIndex].Modifiers;
		MappingContext->Mappings[KeyIndex].Key = SelectedKey.Key;
		MappingContext->Mappings[KeyIndex].Modifiers = KeyModifier;
		BlastCharacter->BlastCharacterMappingContext = MappingContext;
		BlastCharacter->BlastCharacterMappingContext->ReloadConfig();
		if (APlayerController* PlayerController = Cast<APlayerController>(BlastCharacter->GetController()))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(MappingContext, 0);
			}
		}
	}


}
