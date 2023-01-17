// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpawner.h"

// Sets default values
AEnemySpawner::AEnemySpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawn Area"));
	RootComponent = SpawnArea;

	// Bind the OnStartEnemySpawn function to the OnComponentBeginOverlap event
	SpawnArea->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawner::OnStartEnemySpawn);

	TArray<AActor*> MyActors;
	FVector Locations = GetActorLocations(MyActors);
}

// Called when the game starts or when spawned
void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Get the box extents of the UBoxComponent
	FVector BoxExtent = SpawnArea->GetScaledBoxExtent();
	// Get the location of the UBoxComponent
	FVector BoxLocation = SpawnArea->GetComponentLocation();

	// Draw a debug box representing the SpawnArea
	DrawDebugBox(GetWorld(), BoxLocation, BoxExtent, FColor::Blue, false, -1, 0, 1);
}

FVector AEnemySpawner::GetActorLocations(TArray<AActor*> Actors)
{
	FVector TotalLocation = FVector::ZeroVector;
	int32 ActorCount = 0;
	for (AActor* Actor : Actors)
	{
		if (Actor->GetRootComponent() != nullptr)
		{
			TotalLocation += Actor->GetRootComponent()->GetComponentLocation();
			ActorCount++;
		}
	}
	if (ActorCount > 0)
	{
		return TotalLocation / ActorCount;
	}
	return FVector(); // return default vector if no valid actors are passed in
}

void AEnemySpawner::OnStartEnemySpawn(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{

}