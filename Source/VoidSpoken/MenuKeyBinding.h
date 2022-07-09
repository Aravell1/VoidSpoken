// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuKeyBinding.generated.h"

/**
 * 
 */
UCLASS()
class VOIDSPOKEN_API UMenuKeyBinding : public UUserWidget
{
	GENERATED_BODY()

public:
	//Color of what the given Image and Text should be
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FColor Tint;

	//Size of the Given Image
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D ImageSize = {40, 40};

	//Selected 2D Texture for the Image (On Canvas) to have
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* ControllerSprite = NULL;
	
	//The String alongside the Image describing the functionallity of this action
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ActionKeyString = "None";

};
