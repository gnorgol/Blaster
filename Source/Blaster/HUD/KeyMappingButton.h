#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CommonActivatableWidget.h"
#include "KeyMappingButton.generated.h"

class UInputMappingContext;
class USaveGame;
UCLASS()
class BLASTER_API UKeyMappingButton : public UCommonActivatableWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UTextBlock* KeyLabelText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		class UInputKeySelector* InputKey1;

	UFUNCTION(BlueprintCallable)
		void SetKey(FInputChord SelectedKey);

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	int32 KeyIndex;

	USaveGame* SaveGameInstance;


};
