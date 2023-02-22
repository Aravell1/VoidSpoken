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

	TArray<AActor*> FoundBosses;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseBoss::StaticClass(), FoundBosses);
	if (FoundBosses.Num() > 0)
	{
		for (int i = 0 ; i < FoundBosses.Num(); i++)
			AddToMap(Cast<ABaseEnemy>(FoundBosses[i]));
	}

	TArray<AActor*> FoundObelisks;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AObelisk::StaticClass(), FoundObelisks);
	if (FoundObelisks.Num() > 0)
	{
		for (int i = 0; i < FoundObelisks.Num(); i++)
		{
			Obelisks.Add(Cast<AObelisk>(FoundObelisks[i]));
			Obelisks[i]->EnableCharge.AddDynamic(this, &ACombatDirector::SetObeliskMode);
			Obelisks[i]->DisableCharge.AddDynamic(this, &ACombatDirector::SetObeliskMode);
		}
	}

	TArray<AActor*> FoundSpawnPoints;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyPortalSpawn::StaticClass(), FoundSpawnPoints);
	if (FoundSpawnPoints.Num() > 0)
	{
		for (int i = 0; i < FoundSpawnPoints.Num(); i++)
		{
			EnemySpawnPoints.Add(Cast<AEnemyPortalSpawn>(FoundSpawnPoints[i]));
			EnemySpawnPoints[i]->OnEnemySpawned.AddDynamic(this, &ACombatDirector::AddToMap);
		}
	}

	GetWorldTimerManager().SetTimer(SpawnEnemiesTimer,
		this,
		&ACombatDirector::SpawnEnemy,
		SpawnTimerDuration);
}

void ACombatDirector::SpawnEnemy()
{
	if (!bObeliskMode)
	{
		TArray<AEnemyPortalSpawn*> SpawnPoints;
		for (int i = 0; i < EnemySpawnPoints.Num(); i++)
		{
			if (FVector::Distance(EnemySpawnPoints[i]->GetActorLocation(), Player->GetActorLocation()) <= EnemySpawnDistance)
				SpawnPoints.Add(EnemySpawnPoints[i]);
		}

		for (int i = 0; i < EnemiesToSpawn; i++)
		{
			int RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);

			SpawnPoints[RandomIndex]->SpawnPortal();

			SpawnPoints.RemoveAt(RandomIndex);
		}
	}

	SpawnTicks++;
	if (SpawnTicks >= InreaseSpawnsThreshold)
	{
		SpawnTicks = 0;
		EnemiesToSpawn++;
	}

	GetWorldTimerManager().SetTimer(SpawnEnemiesTimer,
		this,
		&ACombatDirector::SpawnEnemy,
		SpawnTimerDuration);
	

}

void ACombatDirector::SpawnObeliskEnemy()
{
	if (ActivatedObelisk)
	{
		TArray<AEnemyPortalSpawn*> SpawnPoints = ActivatedObelisk->NearbySpawnPoints;
		int SpawnCount = EnemiesToSpawn - ObeliskSpawns;
		if (SpawnCount > 0)
		{
			for (int i = 0; i < SpawnCount; i++)
			{
				int RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
				SpawnPoints[RandomIndex]->SpawnPortal();

				SpawnPoints.RemoveAt(RandomIndex);

				ObeliskSpawns++;
			}
		}
	}
}

void ACombatDirector::CalculateEnemyActions()
{
	EnemiesInCombat = 0;

	for (int i = 0; i < Enemies.Num(); i++)
	{
		if (Enemies[i].Enemy->bInCombat)
		{
			if (Enemies[i].Enemy->bCanAttack)
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
						if (Enemies[i].Enemy->GetEnemyType() == EEnemyType::Melee)
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

void ACombatDirector::AddToMap(ABaseEnemy* Enemy)
{
	FEnemyData NewEnemy;
	NewEnemy.Enemy = Enemy;

	Enemy->OnDeathTrigger.AddDynamic(this, &ACombatDirector::RemoveEnemy);

	Enemies.Add(NewEnemy);

	if (bObeliskMode)
		Enemy->EnterCombat(Player, false);
}

void ACombatDirector::RemoveEnemy(ABaseEnemy* Enemy)
{
	for (int i = 0; i < Enemies.Num(); i++)
	{
		if (Enemies[i].Enemy == Enemy)
		{
			Enemies.RemoveAt(i);

			if (bObeliskMode)
			{
				ObeliskSpawns--;
				if (ObeliskSpawns < 0)
					ObeliskSpawns = 0;
				SpawnObeliskEnemy();
			}

			return;
		}
	}
}

void ACombatDirector::SetObeliskMode(AObelisk* Obelisk)
{
	bObeliskMode = !bObeliskMode;

	if (bObeliskMode)
	{
		ActivatedObelisk = Obelisk;
		for (int i = 0; i < Obelisks.Num(); i++)
		{
			Obelisks[i]->SetCanBeginCharging(false);
		}

		SpawnObeliskEnemy();
	}
	else
	{
		ActivatedObelisk = nullptr;
		for (int i = 0; i < Obelisks.Num(); i++)
		{
			if (Obelisks[i] != ActivatedObelisk && Obelisks[i]->GetObeliskState() == EActivationState::Inactive)
			{
				Obelisks[i]->SetCanBeginCharging(true);
			}
		}
	}
}
