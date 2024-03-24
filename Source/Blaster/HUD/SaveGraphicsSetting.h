// Guillaume Norgol. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "UpscaleMode.h"
#include "NISLibrary.h"
#include "SaveGraphicsSetting.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API USaveGraphicsSetting : public USaveGame
{
	GENERATED_BODY()

public:
	USaveGraphicsSetting();
	UPROPERTY()
		EUpscaleMode CurrentUpscaleMode;

	UPROPERTY()
		UNISMode CurrentNISMode = UNISMode::Custom;

	UPROPERTY()
		float NISSharpness = 0.0f;
			
		
	
};
