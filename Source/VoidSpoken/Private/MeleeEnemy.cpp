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

}

void AMeleeEnemy::ChangeStates(MeleeBehaviourState NewState)
{
	CurrentBehaviour = NewState;
}

void AMeleeEnemy::ChangePatrolPoints()
{
	//if in patrol, move to next point
	// if ReturnToStart && index is at the end, set index back to 0
	// otherwise, flip direction
}

void AMeleeEnemy::ChasePlayer()
{
	// go after player
		// if player is in attackrange, go to attack
		//if the player goes out of chase range, change to search state
}

void AMeleeEnemy::Search()
{
	// go to players last position scene and wait
		// if player is not scene, go back to patrol
		// if player is scene, go back to chase
}

void AMeleeEnemy::Die() 
{
	//selecet death animation
	//if head explosion, set a damage raidius, if player is in range deal DAMAGE TO THEM
	//item drop chance

}

void AMeleeEnemy::DropItem()
{
	//random chance to drop item
		// if true
			// select a item to drop
			// drop item
		
}

void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();

	//do action based on state

		
	//if chase,

	// if Attack, Execute Attack

	// if Search, execute Search method

	// if dead, execute Die method

}