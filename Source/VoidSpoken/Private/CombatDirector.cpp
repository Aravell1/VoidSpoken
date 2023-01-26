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
}

void ACombatDirector::CalculateEnemyActions()
{
	for (auto& Elem : Enemies)
	{
		if (Elem.Key == nullptr)
		{
			Enemies.Remove(Elem.Key);
		}
		else if (Elem.Key->bInCombat)
		{
			Elem.Value = UGameplayStatics::GetTimeSeconds(GetWorld()) - Elem.Key->TimeOfLastAttack;
		}
		else
		{
			Elem.Value = -1;
		}
	}
}

ABaseEnemy* ACombatDirector::GetBestEnemy(float &Value)
{
	ABaseEnemy* HighestValueEnemy = Enemies.begin()->Key;
	Value = Enemies.begin()->Value;

	for (auto& Elem : Enemies)
	{
		if (bDebugMode && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Blue, "HVE: " + HighestValueEnemy->GetName() + ": " + FString::SanitizeFloat(*Enemies.Find(HighestValueEnemy)));
			GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Red, "Compare: " + Elem.Key->GetName() + ": " + FString::SanitizeFloat(Elem.Value));
		}
		
		if (Elem.Key == nullptr)
		{
			Enemies.Remove(Elem.Key);
		}
		else if (Elem.Value > *Enemies.Find(HighestValueEnemy))
		{
			HighestValueEnemy = Elem.Key;
			Value = Elem.Value;
		}
	}

	return HighestValueEnemy;
}

void ACombatDirector::TriggerEnemyAttack()
{
	if (Enemies.Num() > 0)
	{
		CalculateEnemyActions();

		float HighestValue = 0;
		ABaseEnemy* HighestValueEnemy = GetBestEnemy(HighestValue);

		if (bDebugMode && GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 0.5f, FColor::Green, "HVE: " + HighestValueEnemy->GetName() + ": " + FString::SanitizeFloat(HighestValue));

		if (HighestValue > MinTimeBetweenAttacks)
			HighestValueEnemy->TriggerAttack();
	}

	GetWorldTimerManager().SetTimer(CombatAttackTimer, this, &ACombatDirector::TriggerEnemyAttack, AttackCheckInterval);
}

void ACombatDirector::AddToMap(ABaseEnemy* Enemy)
{
	Enemies.Add(Enemy, 0);
}
