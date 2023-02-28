// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEnemy.h"
#include "EnemyPortalSpawn.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSpawnDelegate, ABaseEnemy*, NewEnemy, bool, ObeliskSpawn);

UCLASS()
class VOIDSPOKEN_API AEnemyPortalSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyPortalSpawn();


	UPROPERTY(EditAnywhere)
		TArray<APatrolPoint*> PatrolPoints;
	
	void SpawnPortal(bool ObeliskSpawn);

	UPROPERTY(EditAnywhere)
		bool bSpawnOnBeginPlay = false;

	FSpawnDelegate OnEnemySpawned;
	bool bEnemySpawning = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly)
		TArray<TSubclassOf<ABaseEnemy>> EnemyTypes;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> Portal;

	void SpawnEnemy();

	FTimerHandle DistanceCheckTimer;

	const float PlayerDistanceToSpawn = 5000;
	const float DelayPortalToEnemy = 2.5f;
	bool bObeliskSpawn = false;
};
