// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "BaseItem.h"
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

	UFUNCTION(BlueprintPure)
		int GetObeliskRequiredKills() { return ObeliskRequiredKills; }

	UFUNCTION(BlueprintCallable)
		void AddSubtractObeliskRequiredKills(int Count) { ObeliskRequiredKills += Count; }

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


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<TSubclassOf<ABaseItem>> ItemTypes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int ItemTypeIndex = 0;

	
	int MaxHeal = 5;
	int MaxFocus = 5;
	int MaxStamina = 2;

	int HealPickup = 0;
	int FocusPickup = 0;
	int StaminaPickup = 0;

private:

	int ObeliskRequiredKills = 2;

public:

	/// <summary>
	/// An actor class that can be spawn in the scene, Note: Setup needed in the editor
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	TSubclassOf<AActor> PlayerActorClass;

};
