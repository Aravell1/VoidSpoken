// Copyright Epic Games, Inc. All Rights Reserved.


#include "VoidSpokenGameModeBase.h"

void AVoidSpokenGameModeBase::AddGatekeeperSpawn(AActor* EnemySpawn)
{
	GatekeeperEnemySpawns.Add(EnemySpawn);
}

void AVoidSpokenGameModeBase::KillGatekeeperSpawns()
{
	if (GatekeeperEnemySpawns.Num() > 0)
	{
		for (int i = 0; i < GatekeeperEnemySpawns.Num(); i++)
		{
			UGameplayStatics::ApplyDamage(GatekeeperEnemySpawns[i], 10000, NULL, NULL, NULL);
		}
	}
}

void AVoidSpokenGameModeBase::SetHealItem(int heal)
{
	HealPickup += heal;

	UE_LOG(LogTemp, Warning, TEXT("HEALING Items: %d"), HealPickup);
}

void AVoidSpokenGameModeBase::SetFocusItem(int focus)
{
	FocusPickup += focus;

	UE_LOG(LogTemp, Warning, TEXT("FOCUS Items: %d"), FocusPickup);
}

void AVoidSpokenGameModeBase::SetStaminaItem(int stamina)
{
	StaminaPickup += stamina;

	UE_LOG(LogTemp, Warning, TEXT("STAMINA Items: %d"), StaminaPickup);	
}


