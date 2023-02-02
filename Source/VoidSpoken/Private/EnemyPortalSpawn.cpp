// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPortalSpawn.h"

// Sets default values
AEnemyPortalSpawn::AEnemyPortalSpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyPortalSpawn::BeginPlay()
{
	Super::BeginPlay();
	
	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	TArray<AActor*> FoundDirectors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACombatDirector::StaticClass(), FoundDirectors);
	if (Cast<ACombatDirector>(FoundDirectors[0]))
	{
		CombatDirector = Cast<ACombatDirector>(FoundDirectors[0]);
	}

	if (!bEnemySpawned)
	{
		if (Player)
		{
			GetWorldTimerManager().SetTimer(DistanceCheckTimer,
				this,
				&AEnemyPortalSpawn::CheckPlayerDistance,
				DistanceCheckInterval);
		}
	}
}

// Called every frame
void AEnemyPortalSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyPortalSpawn::CheckPlayerDistance()
{
	if (FVector::Distance(GetActorLocation(), Player->GetActorLocation()) <= PlayerDistanceToSpawn)
	{
		bEnemySpawned = true;
		SpawnPortal();

		GetWorldTimerManager().SetTimer(DistanceCheckTimer,
			this,
			&AEnemyPortalSpawn::SpawnEnemy,
			DelayPortalToEnemy);
	}

	if (!bEnemySpawned)
	{
		if (Player)
		{
			GetWorldTimerManager().SetTimer(DistanceCheckTimer,
				this,
				&AEnemyPortalSpawn::CheckPlayerDistance,
				DistanceCheckInterval);
		}
	}
}

void AEnemyPortalSpawn::SpawnPortal()
{
	if (Portal)
	{
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = GetActorRotation();
		FActorSpawnParameters SpawnInfo;

		AActor* SpawnedPortal = GetWorld()->SpawnActor<AActor>(Portal, SpawnLocation, SpawnRotation, SpawnInfo);
		SpawnedPortal->SetLifeSpan(5);
	}
}

void AEnemyPortalSpawn::SpawnEnemy()
{
	if (EnemyType)
	{
		FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100;
		FRotator SpawnRotation = GetActorRotation();
		FActorSpawnParameters SpawnInfo;

		ABaseEnemy* SpawnedEnemy = GetWorld()->SpawnActor<ABaseEnemy>(EnemyType, SpawnLocation, SpawnRotation, SpawnInfo);
		SpawnedEnemy->PatrolPoints = PatrolPoints;

		CombatDirector->AddToMap(SpawnedEnemy);
	}

	Destroy();
}

