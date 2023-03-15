// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEnemy.h"
#include "PlayerCharacter.h"
#include "BaseBoss.h"
#include "EnemyPortalSpawn.h"
#include "Obelisk.h"
#include "../VoidSpokenGameModeBase.h"
#include "CombatDirector.generated.h"

USTRUCT()
struct FEnemyData
{
	GENERATED_USTRUCT_BODY()

	ABaseEnemy* Enemy = nullptr;
	float EnemyValue = 0;
	bool SpawnedEnemy = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCombatDirectorDelegate, bool, bInCombatChange);

UCLASS()
class VOIDSPOKEN_API ACombatDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACombatDirector();

	UPROPERTY(BlueprintAssignable)
		FCombatDirectorDelegate OnInCombatChanged;

	UFUNCTION()
		void AddToMap(ABaseEnemy* Enemy, bool ObeliskSpawn);
	UFUNCTION()
		void RemoveEnemy(ABaseEnemy* Enemy);

	UFUNCTION(BlueprintPure)
		bool GetInCombat() { return bInCombat; }

	UFUNCTION(BlueprintPure)
		int GetRemainingObeliskCount() { return Obelisks.Num(); }

	UFUNCTION()
		void EnableObeliskMode(AObelisk* Obelisk);

	UFUNCTION()
		void DisableObeliskMode(AObelisk* Obelisk);

	UPROPERTY(EditAnywhere)
		bool bDebugMode = false;

	UPROPERTY(EditAnywhere)
		float AttackCheckInterval = 1.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	
	TArray<FEnemyData> Enemies;
	APlayerCharacter* Player;
	AVoidSpokenGameModeBase* GameMode;

	TArray<AEnemyPortalSpawn*> EnemySpawnPoints;
	TArray<AObelisk*> Obelisks;
	AObelisk* ActivatedObelisk;
	void SpawnEnemy();
	void SpawnObeliskEnemy();
	bool bObeliskMode = false;
	bool bAttackOnSpawn = false;

	int EnemySpawns = 0;
	const int EnemyHardCap = 6;
	int MaxEnemiesSpawned = 3;
	int EnemiesToSpawn = 2;
	int SpawnTicks = 0;
	const int InreaseSpawnsThreshold = 5;
	const float EnemySpawnDistance = 1500.0f;
	const float AdditionalSpawnDistance = 500.0f;
	FTimerHandle SpawnEnemiesTimer;
	FTimerHandle SpawnDelayTimer;
	const float SpawnDelayDuration = 0.5f;
	const float SpawnTimerDuration = 60.0f;

	void CalculateEnemyActions();
	float GetEnemyAngle(int Index);
	int GetBestEnemy(float &Value);

	void TriggerEnemyAttack();

	FTimerHandle CombatAttackTimer;
	const float MinTimeBetweenAttacks = 5.0f;

	int DirectionCounter = 2;
	const int DirectionThreshold = 2;
	const float DistanceMultiplier = 50;

	float EnemiesInCombat = 0;

	float MeleeDistance = 100.0f;
	TArray<FVector> EnemyPositions;

	bool bInCombat = false;
};
