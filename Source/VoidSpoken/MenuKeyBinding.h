#pragma once

#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "SlateBasics.h"
#include "SlateExtras.h"
#include "MenuKeyBinding.generated.h"

/**
 * 
 */

UCLASS(Blueprintable)
class UMenuKeyBinding : public UUserWidget
{
	GENERATED_BODY()

private:
	//Image Widget Object Reference
	UPROPERTY(VisibleInstanceOnly)
		UImage* Image;

	//The TextBox Object Reference
	UPROPERTY(VisibleInstanceOnly)
		UTextBlock* ActionMap;

public:
	//Color of what the given Image and Text should be
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSlateBrush ImageParameters;

	//Size of the Given Image
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D ImageSize;

	//The String alongside the Image describing the functionallity of this action
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ActionString;

	//Functions
	virtual void NativePreConstruct(bool IsDesignTime);
	virtual void NativeConstruct();
};
