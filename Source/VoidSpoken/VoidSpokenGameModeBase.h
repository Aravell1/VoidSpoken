// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "JsonLibrary.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "VoidSpokenGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class VOIDSPOKEN_API AVoidSpokenGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	void AddGatekeeperSpawn(AActor* EnemySpawn);
	void KillGatekeeperSpawns();

	UPROPERTY(BlueprintReadWrite)
		TArray<AActor*> GatekeeperEnemySpawns;
	

	UFUNCTION()
	void SetHealItem(int health);

	UFUNCTION(BlueprintCallable)
	int GetHealItem()
	{
		return HealPickup;
	}

	UFUNCTION()
	void SetFocusItem(int focus);

	UFUNCTION(BlueprintCallable)
	int GetFocusItem()
	{
		return FocusPickup;
	}

	UFUNCTION()
	void SetStaminaItem(int stamina);


	UFUNCTION(BlueprintCallable)
	int GetStaminaItem()
	{
		return StaminaPickup;
	}

	UFUNCTION()
	void PickupFull();

	

	int MaxHeal = 5;
	int MaxFocus = 5;
	int MaxStamina = 2;

	int HealPickup = 0;
	int FocusPickup = 0;
	int StaminaPickup = 0;

private:


public:

	/// <summary>
	/// Get Reference to the JsonLibrary
	/// </summary>
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "JSON")
	UJsonLibrary* JsonLibrary;

	/// <summary>
	/// An actor class that can be spawn in the scene, Note: Setup needed in the editor
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TSubclassOf<AActor> PlayerActorClass;

	/// <summary>
	/// Call this function at Events
	/// </summary>
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SpawnPlayersFromData();

};
