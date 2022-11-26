// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeEnemy.h"
#include "Kismet/GameplayStatics.h"

//AMeleeEnemy::AMeleeEnemy() 
//{
//	/*PatrolIndex = 0;
//	*/
//}

AMeleeEnemy::AMeleeEnemy()
{
	//find the player if null
	//PlayerTarget = UGameplayStatics::GetActorOfClass(GetWorld(), TSubClassOf<APlayerController>);

	AttackRange = 50.0f;
}

void AMeleeEnemy::ChangeStates(MeleeBehaviourState NewState)
{
	CurrentBehaviour = NewState;
}

void AMeleeEnemy::ChangePatrolPoints()
{

}

void AMeleeEnemy::ChasePlayer()
{

}

void AMeleeEnemy::Search()
{

}

void AMeleeEnemy::Die() 
{

}

void AMeleeEnemy::DropItem()
{

}

void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();

}