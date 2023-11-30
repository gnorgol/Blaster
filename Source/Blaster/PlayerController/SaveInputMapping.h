// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include <GameFramework/PlayerInput.h>
#include "EnhancedActionKeyMapping.h"
#include "SaveInputMapping.generated.h"


/**
 * 
 */
UCLASS()
class BLASTER_API USaveInputMapping : public USaveGame
{
	GENERATED_BODY()

public:
	USaveInputMapping();
	UPROPERTY()
	TArray<FEnhancedActionKeyMapping> EnhancedActionMappings;

	
};
