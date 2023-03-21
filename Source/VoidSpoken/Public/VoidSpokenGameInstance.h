// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PlayerCharacter.h"
#include "MasterSaveGame.h"
#include "Engine/World.h"
#include "BaseEntity.h"
#include "Obelisk.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "VoidSpokenGameInstance.generated.h"

/**
 * 
 */

UCLASS()
class VOIDSPOKEN_API UVoidSpokenGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	#pragma region Level Loading Functions

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> LoadingScreenWidget;

	const FString MapDirectory = "/Game/Maps/Maps/";

	UUserWidget* LoadingScreen;

	UFUNCTION(BlueprintImplementableEvent)
	void CreateLoadingScreen();

	UFUNCTION(BlueprintCallable)
	void LoadLevel(const FString& MapToLoad) {
		UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
		LoadingScreen = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), LoadingScreenWidget);
		if (LoadingScreen)
			LoadingScreen->AddToViewport();
		LoadPackageAsync(MapDirectory + MapToLoad,
			FLoadPackageAsyncDelegate::CreateLambda([=](const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result) {
				if (Result == EAsyncLoadingResult::Succeeded) AsyncLevelLoadedFinished(MapToLoad);
			}
			), 0, PKG_ContainsMap);
	}

	void AsyncLevelLoadedFinished(const FString& LevelName) {
		UGameplayStatics::OpenLevel(UGameplayStatics::GetPlayerController(GetWorld(), 0), FName(*LevelName));
		//UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
	}

};
