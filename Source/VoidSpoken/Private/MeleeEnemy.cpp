// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeEnemy.h"

//AMeleeEnemy::AMeleeEnemy() 
//{
//	/*PatrolIndex = 0;
//	*/
//}

AMeleeEnemy::AMeleeEnemy()
{
	//find the player if null
	//PlayerTarget = UGameplayStatics::GetActorOfClass(GetWorld(), TSubClassOf<APlayerController>);
	RootComponent = GetCapsuleComponent();
	if (!GetMesh()->SkeletalMesh)
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshContainer(TEXT("/Game/Blueprints/Enemies/BasicMeleeEnemy/Components/Vampire_A_Lusth.Vampire_A_Lusth"));
		if (MeshContainer.Succeeded())
		{
			GetMesh()->SetSkeletalMesh(MeshContainer.Object);

			GetMesh()->SetupAttachment(GetCapsuleComponent());

			//GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
			//GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
		}
	}

	// general states
	SetAttack(5);
	SetWalkSpeed(10);
	SetRunSpeed(20);
	SetDamageMultiplier(1.5);


}

void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();
	

	if (!AIController) {
		AIController = GetController<AMeleeEnemyAIController>();
	}
	if (AIController) {
		//AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AMeleeEnemyAIController::OnMoveCompleted)
		//AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AMeleeEnemy::OnMoveCompleted);
	}

	PatrolIndex = -1;
	ChangePatrolPoints();
	//do action based on state
	Direction = 1;

}

void AMeleeEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
		
	//if chase,
	switch (CurrentBehaviour) {
		case Patrol:
			if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle) {
				ChangePatrolPoints();
			}

			if (FVector::Distance(this->GetActorLocation(), PlayerTarget->GetActorLocation()) < ChaseRange)
				CurrentBehaviour = Chase;

			break;
		case Chase:

			break;
	// if Attack, Execute Attack
		case AttackPlayer:

			break;
		case Dead:

			break;

	}

	// if Search, execute Search method

	// if dead, execute Die method
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
	if (PatrolIndex == PatrolPoints.Num() || PatrolIndex == -1)
	{
		if(ReturnToStart)
			PatrolIndex = 0;
		else {
			Direction *= -1;
		}
	}
	else 
	{
		PatrolIndex += Direction;
	}

	AIController->MoveToActor(PatrolPoints[PatrolIndex], LocationRange);

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

void AMeleeEnemy::TakeDamage(float DMG) {
	//Health -= DMG;
	/*if (Health < 0) {
		Die();
	}*/
}
