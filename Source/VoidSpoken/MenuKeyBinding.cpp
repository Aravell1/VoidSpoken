// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuKeyBinding.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Styling/SlateBrush.h"

void UMenuKeyBinding::NativePreConstruct(bool IsDesignTime) {
	Super::PreConstruct(IsDesignTime);
	Image->SetBrush(ImageParameters);

	ActionMap->SetColorAndOpacity(ImageParameters.TintColor);
	ActionMap->SetText(FText::FromString(": " + ActionString));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PreConstucted!"));
}

void UMenuKeyBinding::NativeConstruct() {
	Super::NativeConstruct();
	Image -> SetBrush(ImageParameters);

	ActionMap -> SetColorAndOpacity(ImageParameters.TintColor);
	ActionMap -> SetText(FText::FromString(": " + ActionString));
}
