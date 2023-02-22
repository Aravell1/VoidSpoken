// Fill out your copyright notice in the Description page of Project Settings.


#include "Obelisk.h"

// Sets default values
AObelisk::AObelisk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	if (!ObeliskMesh)
	{
		ObeliskMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Obelisk Mesh"));
		RootComponent = ObeliskMesh;
	}

	if (!ActivationSphere)
	{
		ActivationSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Activation Sphere"));
		ActivationSphere->SetupAttachment(RootComponent);
		ActivationSphere->SetSphereRadius(PlayerDetectionRadius);
		ActivationSphere->SetGenerateOverlapEvents(true);
	}
}

void AObelisk::SetObeliskState(EActivationState NewState)
{
	ObeliskState = NewState;

	switch (ObeliskState)
	{
	case EActivationState::Charging:
		EnableCharge.Broadcast(this);
		ActivationSphere->SetSphereRadius(EnemyDetectionRadius);
		break;

	case EActivationState::Activated:
		DisableCharge.Broadcast(this);
		GameMode->ObeliskCount--;
		ActivationSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

void AObelisk::AddCharge(ABaseEnemy* EnemyTrigger)
{
	if (GetObeliskState() == EActivationState::Charging)
	{
		if (GameMode)
			ObeliskCharge += GameMode->ObeliskCount;

		UpdateChargeWidget.Broadcast();
		
		if (ObeliskCharge >= TotalRequiredCharge)
			SetObeliskState(EActivationState::Activated);
	}
}

// Called when the game starts or when spawned
void AObelisk::BeginPlay()
{
	Super::BeginPlay();
	
	GameMode = Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode)
		GameMode->ObeliskCount++;

	if (ActivationSphere)
		ActivationSphere->OnComponentBeginOverlap.AddDynamic(this, &AObelisk::OnComponentBeginOverlap);
	if (ActivationSphere)
		ActivationSphere->OnComponentEndOverlap.AddDynamic(this, &AObelisk::OnComponentEndOverlap);
}

void AObelisk::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetObeliskState() != EActivationState::Activated)
	{
		if (GetObeliskState() == EActivationState::Inactive)
		{
			if (OtherActor->GetOwner())
				OtherActor = OtherActor->GetOwner();

			if (OverlappedComponent == ActivationSphere && Cast<APlayerCharacter>(OtherActor))
			{
				SetObeliskState(EActivationState::Charging);
			}
		}

		if (ABaseEnemy* OtherEnemy = Cast<ABaseEnemy>(OtherActor))
		{
			TArray<UObject*> BoundObjects = OtherEnemy->OnDeathTrigger.GetAllObjects();
			if (!BoundObjects.Contains(this))
				OtherEnemy->OnDeathTrigger.AddDynamic(this, &AObelisk::AddCharge);
		}
	}
}

void AObelisk::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (GetObeliskState() != EActivationState::Activated)
	{
		if (ABaseEnemy* OtherEnemy = Cast<ABaseEnemy>(OtherActor))
		{
			OtherEnemy->OnDeathTrigger.RemoveDynamic(this, &AObelisk::AddCharge);
		}
	}
}

// Called every frame
void AObelisk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

