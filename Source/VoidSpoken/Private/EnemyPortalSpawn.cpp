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

	if (bSpawnOnBeginPlay)
		SpawnPortal(false);
}

// Called every frame
void AEnemyPortalSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyPortalSpawn::SpawnPortal(bool ObeliskSpawn)
{
	if (Portal)
	{
		bObeliskSpawn = ObeliskSpawn;
		FVector SpawnLocation = GetActorLocation() + FVector(0, 0, 95);
		FRotator SpawnRotation = GetActorRotation();
		FActorSpawnParameters SpawnInfo;

		AActor* SpawnedPortal = GetWorld()->SpawnActor<AActor>(Portal, SpawnLocation, SpawnRotation, SpawnInfo);
		SpawnedPortal->SetLifeSpan(5);
		bEnemySpawning = true;

		GetWorldTimerManager().SetTimer(DistanceCheckTimer,
			this,
			&AEnemyPortalSpawn::SpawnEnemy,
			DelayPortalToEnemy);
	}
}

void AEnemyPortalSpawn::SpawnEnemy()
{
	if (EnemyTypes.Num() > 0)
	{
		int RandomType = FMath::RandRange(0, EnemyTypes.Num() - 1);

		FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100 + FVector(0, 0, 95);
		FRotator SpawnRotation = GetActorRotation();
		FActorSpawnParameters SpawnInfo;

		ABaseEnemy* SpawnedEnemy = GetWorld()->SpawnActor<ABaseEnemy>(EnemyTypes[RandomType], SpawnLocation, SpawnRotation, SpawnInfo);
		SpawnedEnemy->PatrolPoints = PatrolPoints;
		bEnemySpawning = false;

		OnEnemySpawned.Broadcast(SpawnedEnemy, bObeliskSpawn);
	}
}

