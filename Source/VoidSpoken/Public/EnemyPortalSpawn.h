// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEnemy.h"
#include "CombatDirector.h"
#include "EnemyPortalSpawn.generated.h"

UCLASS()
class VOIDSPOKEN_API AEnemyPortalSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyPortalSpawn();

	UPROPERTY(EditAnywhere)
		TSubclassOf<ABaseEnemy> EnemyType;

	UPROPERTY(EditAnywhere)
		TArray<APatrolPoint*> PatrolPoints;
	
	bool bEnemySpawned = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	AActor* Player;
	
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> Portal;

	void CheckPlayerDistance();
	void SpawnPortal();
	void SpawnEnemy();

	FTimerHandle DistanceCheckTimer;

	const float PlayerDistanceToSpawn = 5000;
	const float DistanceCheckInterval = 1.0f;
	const float DelayPortalToEnemy = 2.5f;

	ACombatDirector* CombatDirector;
};
