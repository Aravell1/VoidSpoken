// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEnemy.h"
#include "PlayerCharacter.h"
#include "CombatDirector.generated.h"

USTRUCT()
struct FEnemyData
{
	GENERATED_USTRUCT_BODY()

	ABaseEnemy* Enemy = nullptr;
	float EnemyValue = 0;
	float EnemyAngle = 0;
};

UCLASS()
class VOIDSPOKEN_API ACombatDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACombatDirector();

	void AddToMap(ABaseEnemy* Enemy);

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

	void CalculateEnemyActions();
	ABaseEnemy* GetBestEnemy(float &Value);
	void SortByAngle(ABaseEnemy* AttackingEnemy);

	void TriggerEnemyMove(ABaseEnemy* EnemyToMove, FVector Destination);
	void TriggerEnemyAttack();

	FTimerHandle CombatAttackTimer;
	const float MinTimeBetweenAttacks = 5.0f;
	const float MinAngleBetweenEnemies = 30.0f;

	float EnemiesInCombat = 0;

};
