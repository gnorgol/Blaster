// Fill out your copyright notice in the Description page of Project Settings.


#include "KeyMappingButton.h"
#include "Components/InputKeySelector.h"
#include "InputModifiers.h"
#include "InputMappingContext.h"
#include "Components/InputKeySelector.h"
#include "Framework/Commands/InputChord.h"
#include <Blaster/Character/BlasterCharacter.h>
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blaster/PlayerController/SaveInputMapping.h"
#include <Kismet/GameplayStatics.h>
#include "EnhancedActionKeyMapping.h"
#include <GameFramework/InputSettings.h>


void UKeyMappingButton::NativeConstruct()
{
	UCommonActivatableWidget::ActivateWidget();
}

void UKeyMappingButton::SetKey(FInputChord SelectedKey)
{
	
	ABlasterCharacter *BlastCharacter = Cast<ABlasterCharacter>(GetOwningPlayerPawn());

	if (BlastCharacter)
	{
		FEnhancedActionKeyMapping OldMappingContext = BlastCharacter->BlastCharacterMappingContext->GetMapping(KeyIndex);
		FEnhancedActionKeyMapping NewMappingContext = OldMappingContext;
		TArray<TObjectPtr<UInputModifier>> KeyModifier;
		
		KeyModifier = OldMappingContext.Modifiers;
		NewMappingContext.Key = SelectedKey.Key;
		NewMappingContext.Modifiers = KeyModifier;
		NewMappingContext.Action = OldMappingContext.Action;
		
		BlastCharacter->SaveInputMapping(OldMappingContext, NewMappingContext);

	}
}

