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

	virtual void Init() override;

	//virtual void Serialize(FArchive& Archive) override;

	UFUNCTION(BlueprintCallable)
	void LoadInventorySaveGame();

	UFUNCTION(BlueprintCallable)
	void SaveInventorySaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadPlayerSaveGame();

	UFUNCTION(BlueprintCallable)
	void SavePlayerSaveGame();

	UFUNCTION(BlueprintCallable)
	void LoadObeliskSaveGame();

	UFUNCTION(BlueprintCallable)
	void SaveObeliskSaveGame();

	//UFUNCTION(BlueprintCallable, Category = "Save Game")
	//FVector GetPlayerLocation() const;

	//AObelisk* GetObelisk(const FString& ObeliskName);

public:

	UFUNCTION(BlueprintCallable)
	void ResetSaveGameData();

	#pragma region Level Loading Functions

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UUserWidget> LoadingScreenWidget;

	FTimerHandle LoadingScreenDelay;
	FString LevelName;

	UFUNCTION(BlueprintCallable)
	void LoadLevel(const FString& MapToLoad) {
		LevelName = MapToLoad;
		UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
		UUserWidget* LoadingScreen = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), LoadingScreenWidget);
		LoadingScreen->AddToViewport();
		LoadPackageAsync("/Game/Maps/Maps/" + LevelName,
			FLoadPackageAsyncDelegate::CreateLambda([=](const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result) {
				if (Result == EAsyncLoadingResult::Succeeded) AsyncLevelLoadedFinished();
			}
			), 0, PKG_ContainsMap);
	}

	void AsyncLevelLoadedFinished() {
		UGameplayStatics::OpenLevel(this, FName(*LevelName));
		UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
	}

	#pragma endregion 
	
private:

	FString CurrentObeliskName = "DefaultObeliskName";
	FString InventorySlotName = "InventorySavedGameName";
	FString PlayerSlotName = "PlayerSavedGameName";
	FString ObeliskSlotName = "ObeliskSavedGameName";

	/* Declare the GetObelisk() function */
	TArray<AObelisk*> GetAllObelisks();
	//AObelisk* GetObeliskByName(const FString& ObeliskName) const;
	//AObelisk* GetObelisk(const FString& ObeliskName);
	//AObelisk* GetObelisk();

	bool DoesSaveGameExist(FString SlotName);

	UPROPERTY()
	UMasterSaveGame* PlayerSaveGame;
};
