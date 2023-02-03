// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventorySystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VOIDSPOKEN_API UInventorySystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventorySystem();

	UFUNCTION()
		void SetHealItem(int heal);

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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	int HealPickup;
	int FocusPickup;
	int StaminaPickup;

	int HealLimit;
	int FocusLimit;
	int StaminaLimit;
};
