// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckPoints.h"

// Sets default values
ACheckPoints::ACheckPoints()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshRadius = 200.0f;

	CheckpointMeshCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Checkpoint Component"));
	CheckpointMeshCollision->InitSphereRadius(MeshRadius);
	CheckpointMeshCollision->SetCollisionProfileName("Trigger");
	RootComponent = CheckpointMeshCollision;

	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Checkpoint Mesh"));
	CheckpointMesh->SetupAttachment(RootComponent);

	CheckpointMeshCollision->OnComponentBeginOverlap.AddDynamic(this, &ACheckPoints::OnPlayerCollide);
}

// Called when the game starts or when spawned
void ACheckPoints::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACheckPoints::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugSphere(GetWorld(), GetActorLocation(), MeshRadius, 20, FColor::Red, false, -1, 0, 1);
}

void ACheckPoints::OnPlayerCollide(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	// Create an array of FPlayerData structs
	TArray<FPlayerData> PlayerDataInfo;

	// Populate the struct with the player's data
	FPlayerData PlayerData;

	// Check if the overlapped actor is the player
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player != nullptr)
	{
		PlayerData.isActive = true;
		PlayerData.UserName = Player->GetName();
		PlayerData.Locations.Add(Player->GetActorLocation());
		PlayerData.CurrentLocation = Player->GetActorLocation();

		// Add the player data to the array
		PlayerDataInfo.Add(PlayerData);

		// Call the WritePlayerData function to save the player data to a json file
		UJsonLibrary::WritePlayerData(PlayerDataInfo);

		// Notify the player that their data has been saved
		//Player->NotifySaved();
	}

	UStatsMasterClass* PlayerStats = Cast<UStatsMasterClass>(OtherActor);
	if (PlayerStats != nullptr)
	{
		PlayerData.Health = PlayerStats->Health;

		// Add the player data to the array
		PlayerDataInfo.Add(PlayerData);

		// Call the WritePlayerData function to save the player data to a json file
		UJsonLibrary::WritePlayerData(PlayerDataInfo);
	}
}

