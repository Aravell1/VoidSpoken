// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Obelisk.h"
#include "MasterSaveGame.generated.h"

USTRUCT(BlueprintType)
struct FObeliskSaveData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obelisk")
	FString ObeliskName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EActivationState ObeliskState;
};

/**
 * 
 */
UCLASS()
class VOIDSPOKEN_API UMasterSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

	//stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health;

	//stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Stamina;

	//stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Focus;

	//players
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	FTransform PlayerTransform;

	//players
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	FVector PlayerLocation;

	//players
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	FRotator PlayerRotation;

	//obelisk
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Obelisk)
	TArray<FObeliskSaveData> ObeliskSaveData;

	//pickups
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int HealPickup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int FocusPickup;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickups")
	int StaminaPickup;

	UFUNCTION(BlueprintCallable)
	void Reset();

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerLocation(const FVector& NewLocation);
};
