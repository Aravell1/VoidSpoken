// Fill out your copyright notice in the Description page of Project Settings.


#include "InventorySystem.h"

// Sets default values for this component's properties
UInventorySystem::UInventorySystem()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	HealAmount = 10.f;
	FocusAmount = 25.f;
	StaminaAmount = 12.5f;

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


void UInventorySystem::UseHealthConsumable()
{
	AVoidSpokenGameModeBase* GM;
	GM = Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GM->HealPickup > 0)
	{
		if (Stats->Health < Stats->GetMaxHealth())
		{
			Stats->Health += HealAmount;
			GM->HealPickup -= 1;

			if (Stats->Health >= Stats->GetMaxHealth())
			{
				Stats->Health = Stats->GetMaxHealth();
			}
		}
	}
	else
		return;
}

void UInventorySystem::UseFocusConsumable()
{
	AVoidSpokenGameModeBase* GM;
	GM = Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GM->FocusPickup > 0)
	{
		if (Stats->FocusPoints < Stats->GetMaxFocus())
		{
			Stats->FocusPoints += FocusAmount;
			GM->FocusPickup -= 1;

			if (Stats->FocusPoints >= Stats->GetMaxFocus())
			{
				Stats->FocusPoints = Stats->GetMaxFocus();
			}
		}
	}
	else
		return;
}

void UInventorySystem::UseStaminaConsumable()
{
	AVoidSpokenGameModeBase* GM;
	GM = Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GM->StaminaPickup > 0)
	{
		if (Stats->Stamina < Stats->GetMaxStamina())
		{
			Stats->Stamina += StaminaAmount;
			GM->StaminaPickup -= 1;

			if (Stats->Stamina >= Stats->GetMaxStamina())
			{
				Stats->Stamina = Stats->GetMaxStamina();
			}
		}
	}
	else
		return;
}