// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonLibrary.generated.h"

typedef TSharedPtr<FJsonObject> JsonObjectPtr;
typedef TSharedPtr<FJsonValue> JsonValuePtr;

/// <summary>
/// player's information 
/// </summary>

USTRUCT(BlueprintType)
struct FPlayerData
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, Category = "Player Data")
		FString UserName;
	UPROPERTY(EditAnywhere, Category = "Player Data")
		int UserNumber;
	UPROPERTY(EditAnywhere, Category = "Player Data")
		float Health;
	UPROPERTY(EditAnywhere, Category = "Player Data")
		bool isActive;
	UPROPERTY(EditAnywhere, Category = "Player Data")
		//TArray<FString> Locations;
		TArray<FVector> Locations;
	UPROPERTY(EditAnywhere, Category = "Player Data")
		FVector CurrentLocation;

};

UCLASS()
class VOIDSPOKEN_API UJsonLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UJsonLibrary();
	/// <summary>
	/// a function used to store base file for the character/weapon/enemy/etc
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Temp")
		static void GenerateBaseFile();

	UFUNCTION(BlueprintCallable, Category = "Temp")
		static bool WriteInventoryData(FString CharName, TArray<AActor*> InventoryItems, TArray<int> InventoryCount);

	UFUNCTION(BlueprintCallable, Category = "Temp")
		static bool ReadInvenotyData(FDateTime& SaveDate, FString& CharName, TArray<FString>& InventoryItems, TArray<int>& InventoryCount);

	//try and get project version
	UFUNCTION(BlueprintCallable, Category = "Temp")
		static bool GetProjectVersion(FString& ProjectVersion);

	UFUNCTION(BlueprintCallable, Category = "Temp")
		static bool ReadPlayerData(TArray<FPlayerData>& OutPlayerData);

	UFUNCTION(BlueprintCallable, Category = "Temp")
		static bool WritePlayerData(TArray<FPlayerData> PlayerDataInfo);

private:

	/// <summary>
	/// a write function that can be reuse again
	/// </summary>
	static bool WriteJsonFile(JsonObjectPtr Object, FString FullPath, bool bCondensed = false);

	/// <summary>
	///a read function that can be reuse again
	/// </summary>
	static JsonObjectPtr ReadJsonFile(FString FullPath);
};