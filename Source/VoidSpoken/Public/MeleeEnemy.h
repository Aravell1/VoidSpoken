// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"
#include "Containers/Array.h"
#include "Math/Vector.h"
#include "MeleeEnemyAIController.h"
#include "MeleeEnemy.generated.h"

UENUM()
enum MeleeBehaviourState
{
	Patrol	UMETA(DisplayName = "Patrol"),
	Chase	UMETA(DisplayName = "Chase"),
	AttackPlayer	UMETA(DisplayName = "AttackPlayer"),
	Search	UMETA(DisplayName = "Search"),
	Dead	UMETA(DisplayName = "Dead")
};

/**
 * 
 */
UCLASS()
class VOIDSPOKEN_API AMeleeEnemy : public ABaseEnemy
{
	GENERATED_BODY()

public:
	AMeleeEnemy();

	void ChangeStates(MeleeBehaviourState NewState);
	void ChangePatrolPoints();
	void ChasePlayer();
	void Search();
	void Die();
	void DropItem();
	void TakeDamage(float DMG);

	UPROPERTY(EditAnywhere)
	bool FindPoints = false;

	UPROPERTY(EditAnywhere)
	bool ReturnToStart = true;

	UPROPERTY(EditAnywhere, Category = PatrolPoints)
	TArray<AActor*> PatrolPoints;

	UPROPERTY(EditAnywhere, Category = ItemDrops)
	TArray<AActor*> ItemDrops;

	UPROPERTY(EditAnywhere)
	AActor* PlayerTarget = nullptr;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "set the very first animation to the head explosion attack"))
	TArray<UAnimMontage*> AttackAnims;

	UPROPERTY(EditAnywhere)
	bool UseHeadExplosion = false;

	UPROPERTY(EditAnywhere)
	float AttackRange = 3.0f;

	UPROPERTY(EditAnywhere)
	float ChaseRange = 10.0f;

	UPROPERTY(EditAnywhere)
	float LocationRange = 5.0f;

	UPROPERTY(EditAnywhere)
	int Direction = 1;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	MeleeBehaviourState CurrentBehaviour = Patrol;
	AMeleeEnemyAIController* AIController;
	int PatrolIndex;
	bool CanAttack;

};

