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
		if (Enemies[i].Enemy->bInCombat && Enemies[i].Enemy->bCanAttack)
		{
			Enemies[i].EnemyValue = 0;

			float TimeSinceLastAttack = UGameplayStatics::GetTimeSeconds(GetWorld()) - Enemies[i].Enemy->TimeOfLastAttack;
			if (TimeSinceLastAttack > MinTimeBetweenAttacks)
			{
				Enemies[i].EnemyValue += (TimeSinceLastAttack) * 10;

				float Angle = GetEnemyAngle(i);

				if (Angle >= 1)
					Enemies[i].EnemyValue += 50 / Angle;
				else
					Enemies[i].EnemyValue += 50;
			}
			else
			{
				Enemies[i].EnemyValue = -1;
			}

			EnemiesInCombat++;
		}
		else
		{
			Enemies[i].EnemyValue = -1;
		}
	}
}

float ACombatDirector::GetEnemyAngle(int Index)
{
	FVector PlayerToEnemy = (Enemies[Index].Enemy->GetActorLocation() - Player->GetActorLocation()).GetSafeNormal();
	FVector PlayerForward = Player->GetActorForwardVector();

	float VectorDot = FVector::DotProduct(PlayerForward, PlayerToEnemy);
	float Angle = FMath::RadiansToDegrees(FMath::Acos(VectorDot));

	if (Angle > 180)
		Angle = FMath::Abs(Angle - 360);

	return Angle;
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
			GEngine->AddOnScreenDebugMessage(-1, AttackCheckInterval, FColor::Blue, "HVE: " + HighestValueEnemy->GetName() + ": " + FString::SanitizeFloat(Enemies[HighestValueIndex].EnemyValue));
			GEngine->AddOnScreenDebugMessage(-1, AttackCheckInterval, FColor::Red, "Compare: " + Enemies[i].Enemy->GetName() + ": " + FString::SanitizeFloat(Enemies[i].EnemyValue));
		}
		
		if (Enemies[i].EnemyValue > Enemies[HighestValueIndex].EnemyValue)
		{
			HighestValueIndex = i;
			EnemyValue = Enemies[i].EnemyValue;
		}
	}
	HighestValueEnemy = Enemies[HighestValueIndex].Enemy;

	return HighestValueEnemy;
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

			if (bDebugMode && GEngine)
				GEngine->AddOnScreenDebugMessage(-1, AttackCheckInterval, FColor::Green, "HVE: " + HighestValueEnemy->GetName() + ": " + FString::SanitizeFloat(HighestEnemyValue));

			if (HighestEnemyValue > 0)
				HighestValueEnemy->TriggerAttack();
			else
				HighestValueEnemy = nullptr;
		}
	}

	GetWorldTimerManager().SetTimer(CombatAttackTimer, this, &ACombatDirector::TriggerEnemyAttack, AttackCheckInterval);
}

void ACombatDirector::AddToMap(ABaseEnemy* Enemy)
{
	FEnemyData NewEnemy;
	NewEnemy.Enemy = Enemy;

	Enemy->OnDestroyed.AddDynamic(this, &ACombatDirector::RemoveEnemy);

	Enemies.Add(NewEnemy);
}

void ACombatDirector::RemoveEnemy(AActor* Enemy)
{
	for (int i = 0; i < Enemies.Num(); i++)
	{
		if (Enemies[i].Enemy == Enemy)
		{
			Enemies.RemoveAt(i);
			return;
		}
	}
}
