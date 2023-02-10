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

	bInCombat = EnemiesInCombat > 0;
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

int ACombatDirector::GetBestEnemy(float &EnemyValue)
{
	EnemyValue = Enemies[0].EnemyValue;
	int HighestValueIndex = 0;

	for (int i = 0; i < Enemies.Num(); i++)
	{
		if (bDebugMode && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, AttackCheckInterval, FColor::Blue, "HVE: " + Enemies[HighestValueIndex].Enemy->GetName() + ": " + FString::SanitizeFloat(Enemies[HighestValueIndex].EnemyValue));
			GEngine->AddOnScreenDebugMessage(-1, AttackCheckInterval, FColor::Red, "Compare: " + Enemies[i].Enemy->GetName() + ": " + FString::SanitizeFloat(Enemies[i].EnemyValue));
		}
		
		if (Enemies[i].EnemyValue > Enemies[HighestValueIndex].EnemyValue && Enemies[i].Enemy->CheckLineOfSight(Player))
		{
			HighestValueIndex = i;
			EnemyValue = Enemies[i].EnemyValue;
		}
	}

	return HighestValueIndex;
}

void ACombatDirector::TriggerEnemyAttack()
{
	if (Enemies.Num() > 0)
	{
		CalculateEnemyActions();

		if (EnemiesInCombat > 0)
		{
			float HighestEnemyValue = 0;
			int HighestValueIndex = GetBestEnemy(HighestEnemyValue);

			if (bDebugMode && GEngine)
				GEngine->AddOnScreenDebugMessage(-1, AttackCheckInterval, FColor::Green, "HVE: " + Enemies[HighestValueIndex].Enemy->GetName() + ": " + FString::SanitizeFloat(HighestEnemyValue));

			int AdditionalDistanceIndex = 0;
			for (int i = 0; i < Enemies.Num(); i++)
			{
				if (Enemies[i].Enemy->bInCombat)
				{
					if (i == HighestValueIndex && HighestEnemyValue > 0)
					{
						Enemies[i].Enemy->TriggerAttack();
					}
					else
					{
						if (Enemies[i].MeleeType)
						{
							if (DirectionCounter >= DirectionThreshold)
							{
								Enemies[i].Enemy->SetCirclePlayer(true, DistanceMultiplier * AdditionalDistanceIndex);
								DirectionCounter = 0;
							}
							else
							{
								Enemies[i].Enemy->SetCirclePlayer(false, DistanceMultiplier * AdditionalDistanceIndex);
								DirectionCounter++;
							}
							AdditionalDistanceIndex++;
						}
						else
							Enemies[i].Enemy->SetCombatIdle();
					}
				}
			}
		}
	}

	GetWorldTimerManager().SetTimer(CombatAttackTimer, this, &ACombatDirector::TriggerEnemyAttack, AttackCheckInterval);
}

void ACombatDirector::AddToMap(ABaseEnemy* Enemy, bool MeleeType)
{
	FEnemyData NewEnemy;
	NewEnemy.Enemy = Enemy;
	NewEnemy.MeleeType = MeleeType;

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

bool ACombatDirector::GetInCombat()
{
	return bInCombat;
}
