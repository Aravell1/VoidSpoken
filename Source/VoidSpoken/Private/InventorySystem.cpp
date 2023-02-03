// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySystem.h"

// Sets default values for this component's properties
UInventorySystem::UInventorySystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	HealPickup = 0;
	FocusPickup = 0;
	StaminaPickup = 0;

	HealLimit = 5;
	FocusLimit = 5;
	StaminaLimit = 2;

}



// Called when the game starts
void UInventorySystem::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventorySystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UInventorySystem::SetHealItem(int heal)
{
	if (HealPickup >= HealLimit)
	{
		UE_LOG(LogTemp, Warning, TEXT("HEALING Full"));
		return;
	}
	else
	{
		HealPickup += heal;

		UE_LOG(LogTemp, Warning, TEXT("HEALING Items: %d"), HealPickup);
	}
}

void UInventorySystem::SetFocusItem(int focus)
{
	FocusPickup += focus;

	UE_LOG(LogTemp, Warning, TEXT("FOCUS Items: %d"), FocusPickup);
}

void UInventorySystem::SetStaminaItem(int stamina)
{
	StaminaPickup += stamina;

	UE_LOG(LogTemp, Warning, TEXT("STAMINA Items: %d"), StaminaPickup);
}