// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "Animation/AnimMontage.h"
//#include "Kismet/GameplayStatics.h"
#include "Containers/Array.h"
#include "MeleeEnemy.generated.h"

UENUM()
enum MeleeBehaviourState
{
	Patrol	UMETA(DisplayName = "Patrol"),
	Chase	UMETA(DisplayName = "Chase"),
	Attack	UMETA(DisplayName = "Attack"),
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

	void BeginPlay();

	UPROPERTY(EditAnywhere)
		bool findPoints = false;

	UPROPERTY(EditAnywhere, Category = PatrolPoints)
	TArray<AActor*> PatrolPoints;

	UPROPERTY(EditAnywhere, Category = ItemDrops)
	TArray<AActor*> ItemDrops;

	UPROPERTY(EditAnywhere)
	AActor* PlayerTarget = nullptr;

	UPROPERTY(EditAnywhere)
	TArray<UAnimMontage*> AttackAnims;

	UPROPERTY(EditAnywhere)
	UAnimMontage* HeadExplosion = nullptr;

	UPROPERTY(EditAnywhere)
	float AttackRange = 0.0f;

private:
	MeleeBehaviourState CurrentBehaviour = Patrol;
	int PatrolIndex;

//
//public:
//	AMeleeEnemy();
// 
//	// list of patrol points
//	TArray<APawn> PatrolPoints;
// 
//	// player target
//	APawn* Player;
// 
//	//list of animations
//	TArray<UAnimMontage> AttackAnims;
// 
//	//head exploding animation
//	UAnimMontage HeadExplosion;
// 
//	//attack range
//	float AttackRange;
//
//private:
//	// enum behaviour states
//	/*enum Behaviour {
//		patrol,
//		chase,
//		attack,
//		search
//	};*/
// 
//	// current behaviour
//	//Behaviour current = Behaviour.patrol;
// 
//	//MeleeBehaviourState CurrentBehaviour = Patrol;
// 
//	// location
//	int PatrolIndex;

//private:
	//change states
	//attack
	//change patrol points
	//death


};

