// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "VersionControl.generated.h"

/**
 * 
 */
UCLASS()
class VOIDSPOKEN_API UVersionControl : public UUserWidget
{
	GENERATED_BODY()

	//Will Always Exist for PreAlpha Builds
	UPROPERTY(VisibleInstanceOnly)
		FString FlatVerstionString = "PreAlphaBuild";

	//Text Box Object Reference that will be Affected
	UPROPERTY(VisibleInstanceOnly)
		UTextBlock* VersionText;

public:
	virtual void NativeConstruct();
};
