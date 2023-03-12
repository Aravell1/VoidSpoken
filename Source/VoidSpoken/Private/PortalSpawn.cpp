// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalSpawn.h"

// Sets default values
APortalSpawn::APortalSpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APortalSpawn::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle,	this, &APortalSpawn::SpawnEnemy, 1.5f);

	TArray<AActor*> FoundDirectors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACombatDirector::StaticClass(), FoundDirectors);
	if (Cast<ACombatDirector>(FoundDirectors[0]))
	{
		CombatDirector = Cast<ACombatDirector>(FoundDirectors[0]);
	}
}

// Called every frame
void APortalSpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APortalSpawn::SpawnEnemy()
{
	if (EnemyArray.Num() > 0)
	{
		int EnemyIndex = FMath::RandRange(0, EnemyArray.Num() - 1);

		FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 150 + FVector(0, 0, 95);
		//FVector SpawnLocation = FVector(GetActorLocation().X + FMath::RandRange(-150.0f, 50.0f), GetActorLocation().Y + FMath::RandRange(-150.0f, 150.0f), GetActorLocation().Z + 95);
		float zLook = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation()).Yaw;
		FRotator SpawnRotation = FRotator(0, zLook, 0);
		FActorSpawnParameters SpawnInfo;
		EnemyClass = EnemyArray[EnemyIndex];
		
		ABaseEnemy* Enemy = GetWorld()->SpawnActor<ABaseEnemy>(EnemyClass, SpawnLocation, SpawnRotation, SpawnInfo);
		Enemy->OnSeePawn(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()))->AddGatekeeperSpawn(Enemy);

		CombatDirector->AddToMap(Enemy, false);

		SetLifeSpan(7);
	}
}

