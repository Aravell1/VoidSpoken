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
