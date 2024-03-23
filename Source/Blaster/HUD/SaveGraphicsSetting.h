// Guillaume Norgol. All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "UpscaleMode.h"
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
		
	
};
