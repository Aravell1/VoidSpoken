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
	bool MeleeType = false;
};

UCLASS()
class VOIDSPOKEN_API ACombatDirector : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACombatDirector();

	void AddToMap(ABaseEnemy* Enemy, bool MeleeType);
	UFUNCTION()
		void RemoveEnemy(AActor* Enemy);

	UFUNCTION(BlueprintCallable)
		bool GetInCombat();

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
	float GetEnemyAngle(int Index);
	int GetBestEnemy(float &Value);

	void TriggerEnemyAttack();

	FTimerHandle CombatAttackTimer;
	const float MinTimeBetweenAttacks = 5.0f;

	int DirectionCounter = 5;
	const int DirectionThreshold = 5;

	float EnemiesInCombat = 0;

	float MeleeDistance = 100.0f;
	TArray<FVector> EnemyPositions;

	bool bInCombat = false;
};
