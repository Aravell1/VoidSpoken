// Copyright Epic Games, Inc. All Rights Reserved.


#include "VoidSpokenGameModeBase.h"

AVoidSpokenGameModeBase::AVoidSpokenGameModeBase()
{

}

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

void AVoidSpokenGameModeBase::SetHealItem(int health)
{
	HealPickup += health;

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

void AVoidSpokenGameModeBase::PickupFull()
{
	UE_LOG(LogTemp, Warning, TEXT("Pickup full"));
	return;
}

void AVoidSpokenGameModeBase::SpawnPlayersFromData()
{
	TArray<FPlayerData> PlayerDataInfo;
	bool bReadSuccess = JsonLibrary->ReadPlayerData(PlayerDataInfo);

	if (bReadSuccess)
	{
		if (PlayerDataInfo.Num() > 0)
		{
			FPlayerData PlayerData = PlayerDataInfo[0];
			FVector SpawnLocation = PlayerData.CurrentLocation;

			UWorld* World = GetWorld();
			if (World)
			{
				TArray<AActor*> FoundActors;
				UGameplayStatics::GetAllActorsOfClass(World, PlayerActorClass, FoundActors);

				if (FoundActors.Num() > 0)
				{
					AActor* PlayerCharacter = FoundActors[0];
					PlayerCharacter->SetActorLocation(SpawnLocation);
				}
				else
				{
					FActorSpawnParameters SpawnParams;
					SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
					World->SpawnActor<AActor>(PlayerActorClass.Get(), SpawnLocation, FRotator::ZeroRotator, SpawnParams);
				}
			}
		}
	}
}

