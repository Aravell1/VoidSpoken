// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
//#include "Animation/AnimMontage.h"
//#include "Kismet/GameplayStatics.h"
#include "Containers/Array.h"
#include "MeleeEnemy.generated.h"

//UENUM()
//enum MeleeBehaviourState
//{
//	Patrol	UMETA(DisplayName = "Patrol"),
//	Chase	UMETA(DisplayName = "Chase"),
//	Attack	UMETA(DisplayName = "Attack"),
//	Search	UMETA(DisplayName = "Search"),
//	Dead	UMETA(DisplayName = "Dead")
//};

/**
 * 
 */
UCLASS()
class VOIDSPOKEN_API AMeleeEnemy : public ABaseEnemy
{
	GENERATED_BODY()

public:
	AMeleeEnemy();

	UPROPERTY(EditAnywhere)
	bool customPoints;
	UPROPERTY(VisibleAnyWhere, EditAnywhere, Category = PatrolPoints)
		TArray<AActor*> PatrolPoints;

private:
	APawn* Player = nullptr;

//
//public:
//	AMeleeEnemy();
//	// list of patrol points
//	TArray<APawn> PatrolPoints;
// 
//	// player target
//	APawn* Player;
//	//list of animations
//	TArray<UAnimMontage> AttackAnims;
//	//head exploding animation
//	UAnimMontage HeadExplosion;
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
//	// current behaviour
//	//Behaviour current = Behaviour.patrol;
//	//MeleeBehaviourState CurrentBehaviour = Patrol;
//	// location
//	int PatrolIndex;

//private:
	//change states
	//attack
	//change patrol points
	//death


};

