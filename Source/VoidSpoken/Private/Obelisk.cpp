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
		ActivationSphere->SetSphereRadius(EnemyDetectionRadius);
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

		SpawnDecal();
		break;

	case EActivationState::Activated:
		DisableCharge.Broadcast(this);
		ActivationSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		if (ChargeZoneDecal)
			ChargeZoneDecal->GetDecal()->SetFadeOut(0, 1.0f, true);
		break;
	}
}

void AObelisk::BeginCharging()
{
	if (GetObeliskState() == EActivationState::Inactive && bCanBeginCharging)
	{
		SetObeliskState(EActivationState::Charging);
	}
}

void AObelisk::Highlight_Implementation(bool bHighlight)
{
	if (GetObeliskState() == EActivationState::Inactive || ObeliskMesh->bRenderCustomDepth)
		ObeliskMesh->SetRenderCustomDepth(bHighlight);
}

void AObelisk::SpawnEnemyParticleEffect(ABaseEnemy* EnemyTrigger)
{
	FVector SpawnLocation = EnemyTrigger->GetActorLocation();
	FRotator SpawnRotation = EnemyTrigger->GetActorRotation();
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = this;

	GetWorld()->SpawnActor<AActor>(EnemyParticleType, SpawnLocation, SpawnRotation, SpawnInfo);
}

void AObelisk::AddCharge()
{
	if (GetObeliskState() == EActivationState::Charging)
	{
		if (GameMode)
			ObeliskCharge++;

		UpdateChargeWidget.Broadcast();
		
		if (ObeliskCharge >= GameMode->GetObeliskRequiredKills())
			SetObeliskState(EActivationState::Activated);
	}
}

// Called when the game starts or when spawned
void AObelisk::BeginPlay()
{
	Super::BeginPlay();
	
	GameMode = Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (ActivationSphere)
		ActivationSphere->OnComponentBeginOverlap.AddDynamic(this, &AObelisk::OnComponentBeginOverlap);
	if (ActivationSphere)
		ActivationSphere->OnComponentEndOverlap.AddDynamic(this, &AObelisk::OnComponentEndOverlap);
}

void AObelisk::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetObeliskState() != EActivationState::Activated)
	{
		if (ABaseEnemy* OtherEnemy = Cast<ABaseEnemy>(OtherActor))
		{
			TArray<UObject*> BoundObjects = OtherEnemy->OnDeathTrigger.GetAllObjects();
			if (!BoundObjects.Contains(this))
				OtherEnemy->OnDeathTrigger.AddDynamic(this, &AObelisk::SpawnEnemyParticleEffect);
		}
	}
}

void AObelisk::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (GetObeliskState() != EActivationState::Activated)
	{
		if (ABaseEnemy* OtherEnemy = Cast<ABaseEnemy>(OtherActor))
		{
			OtherEnemy->OnDeathTrigger.RemoveDynamic(this, &AObelisk::SpawnEnemyParticleEffect);
		}
	}
}

// Called every frame
void AObelisk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AObelisk::SpawnDecal()
{
	if (ChargeZoneDecalClass)
	{
		FActorSpawnParameters SpawnInfo;
		ChargeZoneDecal = GetWorld()->SpawnActor<ADecalActor>(ChargeZoneDecalClass, GetActorLocation(), GetActorRotation(), SpawnInfo);
	}
}
