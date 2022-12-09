// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalSpawn.h"

// Sets default values
APortalSpawn::APortalSpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!BoxMesh)
		BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Box Mesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh>Box(TEXT("StaticMesh'/Game/Maps/Meshes/SM_ChamferCube.SM_ChamferCube'"));

	if (Box.Succeeded())
	{
		BoxMesh->SetStaticMesh(Box.Object);
	}
	RootComponent = BoxMesh;
}

// Called when the game starts or when spawned
void APortalSpawn::BeginPlay()
{
	Super::BeginPlay();
	
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle,	this, &APortalSpawn::SpawnEnemy, 1.5f);
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

		FVector SpawnLocation = FVector(GetActorLocation().X + FMath::RandRange(-150.0f, 50.0f), GetActorLocation().Y + FMath::RandRange(-150.0f, 150.0f), GetActorLocation().Z + 200);
		float zLook = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation()).Yaw;
		FRotator SpawnRotation = FRotator(0, 0, zLook);
		FActorSpawnParameters SpawnInfo;
		EnemyClass = EnemyArray[EnemyIndex];
		
		ABaseEnemy* Enemy = GetWorld()->SpawnActor<ABaseEnemy>(EnemyClass, FVector(0, 0, -50000), SpawnRotation, SpawnInfo);
		Enemy->SetActorLocation(SpawnLocation + FVector(0, 0, 100));
		//Make enemy chase player
		Enemy->AttackTarget = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()))->AddGatekeeperSpawn(Enemy);

		SetLifeSpan(7);
	}
	else if (EnemyBPArray.Num() > 0)
	{
		int EnemyIndex = FMath::RandRange(0, EnemyBPArray.Num() - 1);

		FVector SpawnLocation = FVector(GetActorLocation().X + FMath::RandRange(-150.0f, 150.0f), GetActorLocation().Y + FMath::RandRange(-150.0f, 150.0f), GetActorLocation().Z);
		float zLook = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation()).Yaw;
		FRotator SpawnRotation = FRotator(0, zLook, 0);
		FActorSpawnParameters SpawnInfo;
		EnemyBPClass = EnemyBPArray[EnemyIndex];
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Blue, EnemyBPClass->GetName());

		ACharacter* Enemy = GetWorld()->SpawnActor<ACharacter>(EnemyBPClass, FVector(0, 0, -50000), SpawnRotation, SpawnInfo);
		Enemy->SetActorLocation(SpawnLocation + FVector(0, 0, 100));
		Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()))->AddGatekeeperSpawn(Enemy);
		//Make enemy chase player
		//Enemy->Attack = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

		SetLifeSpan(7);
	}

}

