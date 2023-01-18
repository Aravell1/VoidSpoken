// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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

	AVoidSpokenGameModeBase();

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

private:

	int HealPickup = 0;
	int FocusPickup = 0;
	int StaminaPickup = 0;

};
