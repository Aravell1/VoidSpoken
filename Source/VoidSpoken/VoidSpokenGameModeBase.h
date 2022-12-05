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

	void AddGatekeeperSpawn(AActor* EnemySpawn);
	void KillGatekeeperSpawns();

private:

	TArray<AActor*> GatekeeperEnemySpawns;
	
};
