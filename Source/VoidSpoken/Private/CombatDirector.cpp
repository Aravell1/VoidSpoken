// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatDirector.h"

// Sets default values
ACombatDirector::ACombatDirector()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ACombatDirector::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(CombatAttackTimer, this, &ACombatDirector::TriggerEnemyAttack, AttackCheckInterval);
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void ACombatDirector::CalculateEnemyActions()
{
	EnemiesInCombat = 0;

	for (int i = 0; i < Enemies.Num(); i++)
	{
		if (!Enemies[i].Enemy)
		{
			Enemies.RemoveAt(i);
		}
		else if (Enemies[i].Enemy->bInCombat && Enemies[i].Enemy->bCanAttack)
		{
			Enemies[i].EnemyValue = UGameplayStatics::GetTimeSeconds(GetWorld()) - Enemies[i].Enemy->TimeOfLastAttack;
			EnemiesInCombat++;
		}
		else
		{
			Enemies[i].EnemyValue = -1;
		}
	}
}

ABaseEnemy* ACombatDirector::GetBestEnemy(float &EnemyValue)
{
	ABaseEnemy* HighestValueEnemy = Enemies[0].Enemy;
	EnemyValue = Enemies[0].EnemyValue;
	int HighestValueIndex = 0;

	for (int i = 0; i < Enemies.Num(); i++)
	{
		if (bDebugMode && GEngine)
		{
			//GEngine->AddOnScreenDebugMessage(-1, AttackCheckInterval, FColor::Blue, "HVE: " + HighestValueEnemy->GetName() + ": " + FString::SanitizeFloat(Enemies[HighestValueIndex].EnemyValue));
			//GEngine->AddOnScreenDebugMessage(-1, AttackCheckInterval, FColor::Red, "Compare: " + Enemies[i].Enemy->GetName() + ": " + FString::SanitizeFloat(Enemies[i].EnemyValue));
		}
		
		if (!Enemies[i].Enemy)
		{
			Enemies.RemoveAt(i);
		}
		else if (Enemies[i].EnemyValue > Enemies[HighestValueIndex].EnemyValue)
		{
			HighestValueIndex = i;
			EnemyValue = Enemies[i].EnemyValue;
		}
	}
	HighestValueEnemy = Enemies[HighestValueIndex].Enemy;

	return HighestValueEnemy;
}

void ACombatDirector::SortByAngle(ABaseEnemy* AttackingEnemy)
{
	for (int i = 0; i < Enemies.Num(); i++)
	{
		if (Enemies[i].Enemy->bInCombat)
		{
			FVector PlayerToEnemy = (Enemies[i].Enemy->GetActorLocation() - Player->GetActorLocation()).GetSafeNormal();
			FVector PlayerForward = Player->GetActorForwardVector();

			float VectorDot = FVector::DotProduct(PlayerForward, PlayerToEnemy);
			float Angle = FMath::RadiansToDegrees(FMath::Acos(VectorDot));
			Angle += 30;
			if (Angle > 360)
				Angle -= 360;

			Enemies[i].EnemyAngle = Angle;
		}
		else
		{
			Enemies[i].EnemyAngle = 500;
		}
	}

	Enemies.Sort([](FEnemyData A, FEnemyData B)
	{
		return A.EnemyAngle < B.EnemyAngle;
	});

	if (bDebugMode && GEngine)
	{
		for (int i = 0; i < Enemies.Num(); i++)
		{
			GEngine->AddOnScreenDebugMessage(-1, AttackCheckInterval, FColor::Blue, "HVE: " + Enemies[i].Enemy->GetName() + ": " + FString::SanitizeFloat(Enemies[i].EnemyAngle));
		}
	}

	for (int i = 1; i < Enemies.Num(); i++)
	{
		if (Enemies[i].EnemyAngle > 360)
			return;

		if (bDebugMode && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, AttackCheckInterval, FColor::Yellow, "HVE: " + Enemies[i].Enemy->GetName() + ": " + FString::SanitizeFloat(Enemies[i].EnemyAngle));
		}

		if (Enemies[i].Enemy->bInCombat && Enemies[i].Enemy->bCanAttack && Enemies[i].Enemy != AttackingEnemy)
		{
			if (Enemies[i].EnemyAngle - Enemies[i - 1].EnemyAngle < MinAngleBetweenEnemies && Enemies[i].Enemy != AttackingEnemy)
			{
				float DistToEnemy = FVector::Distance(Player->GetActorLocation(), Enemies[i].Enemy->GetActorLocation());

				FVector Direction = (Enemies[i - 1].Enemy->GetActorLocation() - Player->GetActorLocation()).GetSafeNormal();
				Direction.RotateAngleAxis(35, FVector::UpVector);

				FVector Destination = Player->GetActorLocation() + Direction * DistToEnemy;

				TriggerEnemyMove(Enemies[i].Enemy, Destination);
			}
		}
	}
}

void ACombatDirector::TriggerEnemyMove(ABaseEnemy* EnemyToMove, FVector Destination)
{
	EnemyToMove->TriggerMove(Destination);
}

void ACombatDirector::TriggerEnemyAttack()
{
	if (Enemies.Num() > 0)
	{
		CalculateEnemyActions();

		if (EnemiesInCombat > 0)
		{
			float HighestEnemyValue = 0;
			ABaseEnemy* HighestValueEnemy = GetBestEnemy(HighestEnemyValue);

			/*if (bDebugMode && GEngine)
				GEngine->AddOnScreenDebugMessage(-1, AttackCheckInterval, FColor::Green, "HVE: " + HighestValueEnemy->GetName() + ": " + FString::SanitizeFloat(HighestEnemyValue));*/

			if (HighestEnemyValue > MinTimeBetweenAttacks)
				HighestValueEnemy->TriggerAttack();
			else
				HighestValueEnemy = nullptr;

			if (EnemiesInCombat > 1)
			{
				SortByAngle(HighestValueEnemy);
			}
		}
	}

	GetWorldTimerManager().SetTimer(CombatAttackTimer, this, &ACombatDirector::TriggerEnemyAttack, AttackCheckInterval);
}

void ACombatDirector::AddToMap(ABaseEnemy* Enemy)
{
	FEnemyData NewEnemy;
	NewEnemy.Enemy = Enemy;

	Enemies.Add(NewEnemy);
}
