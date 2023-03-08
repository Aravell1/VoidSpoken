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
	if (!GameMode)
		GameMode = Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	TArray<AActor*> FoundBosses;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseBoss::StaticClass(), FoundBosses);
	if (FoundBosses.Num() > 0)
	{
		for (int i = 0 ; i < FoundBosses.Num(); i++)
			AddToMap(Cast<ABaseEnemy>(FoundBosses[i]), false);
	}

	TArray<AActor*> FoundObelisks;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AObelisk::StaticClass(), FoundObelisks);
	if (FoundObelisks.Num() > 0)
	{
		for (int i = 0; i < FoundObelisks.Num(); i++)
		{
			Obelisks.Add(Cast<AObelisk>(FoundObelisks[i]));
			Obelisks[i]->EnableCharge.AddDynamic(this, &ACombatDirector::EnableObeliskMode);
			Obelisks[i]->DisableCharge.AddDynamic(this, &ACombatDirector::DisableObeliskMode);
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
	if (Obelisks.Num() > 0)
	{
		bAttackOnSpawn = true;

		if (!bObeliskMode)
		{
			TArray<AEnemyPortalSpawn*> SpawnPoints;
			float MaxSpawnDistance = EnemySpawnDistance + AdditionalSpawnDistance * EnemiesToSpawn;
			for (int i = 0; i < EnemySpawnPoints.Num(); i++)
			{
				if (FVector::Distance(EnemySpawnPoints[i]->GetActorLocation(), Player->GetActorLocation()) <= MaxSpawnDistance)
					SpawnPoints.Add(EnemySpawnPoints[i]);
			}

			int SpawnCount = EnemiesToSpawn;
			int MaxEnemies;
			if (MaxEnemiesSpawned < EnemyHardCap)
				MaxEnemies = MaxEnemiesSpawned;
			else
				MaxEnemies = EnemyHardCap;

			if (SpawnCount + EnemySpawns > MaxEnemies)
				SpawnCount = MaxEnemies - EnemySpawns;

			if (SpawnCount > 0)
			{
				if (SpawnPoints.Num() > SpawnCount)
				{
					for (int i = 0; i < SpawnCount; i++)
					{
						int RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);

						SpawnPoints[RandomIndex]->SpawnPortal(true);

						SpawnPoints.RemoveAt(RandomIndex);
					}
				}
				else if (SpawnPoints.Num() > 0)
				{
					for (int i = 0; i < SpawnPoints.Num(); i++)
					{
						int RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);

						SpawnPoints[RandomIndex]->SpawnPortal(true);

						SpawnPoints.RemoveAt(RandomIndex);
					}
				}
			}
		}
		else
		{
			SpawnObeliskEnemy();
		}

		SpawnTicks++;
		if (SpawnTicks >= InreaseSpawnsThreshold)
		{
			SpawnTicks = 0;
			EnemiesToSpawn++;
			MaxEnemiesSpawned = EnemiesToSpawn * 2;
		}

		GetWorldTimerManager().SetTimer(SpawnEnemiesTimer,
			this,
			&ACombatDirector::SpawnEnemy,
			SpawnTimerDuration);
	}
}

void ACombatDirector::SpawnObeliskEnemy()
{
	if (ActivatedObelisk && bObeliskMode)
	{
		TArray<AEnemyPortalSpawn*> SpawnPoints = ActivatedObelisk->NearbySpawnPoints;
		int SpawnCount = EnemiesToSpawn - EnemySpawns;
		if (SpawnCount > SpawnPoints.Num())
			SpawnCount = SpawnPoints.Num();

		int MaxEnemies;
		if (MaxEnemiesSpawned < EnemyHardCap)
			MaxEnemies = MaxEnemiesSpawned;
		else
			MaxEnemies = EnemyHardCap;

		if (SpawnCount + EnemySpawns > MaxEnemies)
			SpawnCount = MaxEnemies - EnemySpawns;

		if (SpawnCount > 0)
		{
			for (int i = 0; i < SpawnCount; i++)
			{
				int RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);

				while (SpawnPoints[RandomIndex]->bEnemySpawning && SpawnCount > 0)
				{
					SpawnPoints.RemoveAt(RandomIndex);
					if (SpawnCount > SpawnPoints.Num())
						SpawnCount = SpawnPoints.Num();
					RandomIndex = FMath::RandRange(0, SpawnPoints.Num() - 1);
				}

				if (SpawnPoints.Num() > 0)
				{
					SpawnPoints[RandomIndex]->SpawnPortal(true);

					SpawnPoints.RemoveAt(RandomIndex);
				}
			}
		}
	}
}

void ACombatDirector::CalculateEnemyActions()
{
	EnemiesInCombat = 0;
	EnemySpawns = 0;

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

		if (Enemies[i].SpawnedEnemy)
			EnemySpawns++;
	}

	if (bInCombat != EnemiesInCombat > 0)
	{
		bInCombat = EnemiesInCombat > 0;

		OnInCombatChanged.Broadcast(bInCombat);
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
		
		if (Enemies[i].EnemyValue > Enemies[HighestValueIndex].EnemyValue && (Enemies[i].Enemy->CheckLineOfSight(Player) || Enemies[i].Enemy->TestPathExists(Player)))
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
				if (Enemies[i].Enemy->bInCombat && !Cast<ABaseBoss>(Enemies[i].Enemy))
				{
					if (i == HighestValueIndex && HighestEnemyValue > 0)
					{
						Enemies[i].Enemy->TriggerAttack();
					}
					else if (Enemies[i].Enemy->bCanAttack)
					{
						if (DirectionCounter >= DirectionThreshold)
						{
							if (Enemies[i].Enemy->GetEnemyType() == EEnemyType::Melee)
							{
								Enemies[i].Enemy->SetCirclePlayer(true, DistanceMultiplier * AdditionalDistanceIndex);
								DirectionCounter = 0;
							}
							else
								Enemies[i].Enemy->SetCombatIdle();
						}
						else
						{
							//Enemies[i].Enemy->SetCirclePlayer(false, DistanceMultiplier * AdditionalDistanceIndex);
							DirectionCounter++;
						}
						AdditionalDistanceIndex++;
					}
				}
			}
		}
	}

	GetWorldTimerManager().SetTimer(CombatAttackTimer, this, &ACombatDirector::TriggerEnemyAttack, AttackCheckInterval);
}

void ACombatDirector::AddToMap(ABaseEnemy* Enemy, bool SpawnedEnemy)
{
	if (Obelisks.Num() <= 0 && !Cast<ABaseBoss>(Enemy))
	{
		UGameplayStatics::ApplyDamage(Enemy, 10000, NULL, NULL, NULL);
		return;
	}

	FEnemyData NewEnemy;
	NewEnemy.Enemy = Enemy;
	NewEnemy.SpawnedEnemy = SpawnedEnemy;

	if (SpawnedEnemy)
		EnemySpawns++;

	Enemy->OnDeathTrigger.AddDynamic(this, &ACombatDirector::RemoveEnemy);

	Enemies.Add(NewEnemy);

	if (bObeliskMode || bAttackOnSpawn)
		Enemy->EnterCombat(Player, false);
}

void ACombatDirector::RemoveEnemy(ABaseEnemy* Enemy)
{
	for (int i = 0; i < Enemies.Num(); i++)
	{
		if (Enemies[i].Enemy == Enemy)
		{
			Enemies.RemoveAt(i);
			EnemySpawns--;

			if (bObeliskMode)
			{
				GetWorldTimerManager().SetTimer(SpawnDelayTimer,
					this,
					&ACombatDirector::SpawnObeliskEnemy,
					SpawnDelayDuration);
			}

			return;
		}
	}
}

void ACombatDirector::DisableObeliskMode(AObelisk* Obelisk)
{
	bObeliskMode = false;
	
	ActivatedObelisk = nullptr;
	for (int i = 0; i < Obelisks.Num() && Obelisks.Num() > 0; i++)
	{
		if (Obelisks[i] == Obelisk || Obelisks[i]->GetObeliskState() == EActivationState::Activated)
		{
			Obelisks.RemoveAt(i);
			i--;
		}
		else if (Obelisks[i]->GetObeliskState() == EActivationState::Inactive)
		{
			Obelisks[i]->SetCanBeginCharging(true);
		}
	}

	if (Obelisks.Num() <= 0)
	{
		for (int j = 0; j < Enemies.Num(); j++)
		{
			if (!Cast<ABaseBoss>(Enemies[j].Enemy))
			{
				UGameplayStatics::ApplyDamage(Enemies[j].Enemy, 10000, NULL, NULL, NULL);
			}
		}
	}

	if (Obelisk->GetObeliskState() == EActivationState::Activated && Obelisks.Num() > 0)
		GameMode->AddSubtractObeliskRequiredKills(2);
}

void ACombatDirector::EnableObeliskMode(AObelisk* Obelisk)
{
	bObeliskMode = true;

	ActivatedObelisk = Obelisk;
	for (int i = 0; i < Obelisks.Num(); i++)
	{
		Obelisks[i]->SetCanBeginCharging(false);
	}

	SpawnObeliskEnemy();
}
