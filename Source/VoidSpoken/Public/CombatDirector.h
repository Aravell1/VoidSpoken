// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEnemy.h"
#include "CombatDirector.generated.h"

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
	
	TMap<ABaseEnemy*, float> Enemies;

	void CalculateEnemyActions();
	ABaseEnemy* GetBestEnemy(float &Value);

	void TriggerEnemyAttack();

	FTimerHandle CombatAttackTimer;
	const float MinTimeBetweenAttacks = 5.0f;

};
