// Guillaume Norgol. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveSensitivity.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API USaveSensitivity : public USaveGame
{
	GENERATED_BODY()
public:
	USaveSensitivity();

	UPROPERTY()
	float MouseSensitivity;

	UPROPERTY()
	float AimSensitivity;
	
};
