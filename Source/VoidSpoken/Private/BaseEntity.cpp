// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEntity.h"

// Sets default values
ABaseEntity::ABaseEntity()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Stats = CreateDefaultSubobject<UStatsMasterClass>("Stats");

	OnTakeAnyDamage.AddDynamic(this, &ABaseEntity::TakeAnyDamage);

	if (!StaggerComponent)
	{
		StaggerComponent = CreateDefaultSubobject<UStaggerComponent>(TEXT("Stagger Component"));
	}
}

void ABaseEntity::OnStaggered()
{
}

// Called when the game starts or when spawned
void ABaseEntity::BeginPlay()
{
	Super::BeginPlay();
	
	if (StaggerComponent->bCanStagger)
		StaggerComponent->StaggerTrigger.AddDynamic(this, &ABaseEntity::OnStaggered);

	if (Stats->Health > Stats->GetMaxHealth())
		Stats->SetMaxHealth(Stats->Health);
}

// Called every frame
void ABaseEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseEntity::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseEntity::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Cast<UDamageTypeStagger>(DamageType) && StaggerComponent->bCanStagger)
	{
		StaggerComponent->AddToStaggerGauge(Damage);
	}
	else if (Cast<UDamageTypeTelekinesis>(DamageType))
	{
		if (StaggerComponent->bCanStagger)
		{
			StaggerComponent->TriggerFullStagger();
		}

		Damage *= 1.5f;

		OnTelekineticHit();
	}

	Damage = FMath::Floor(Damage * (25 / (25 + Stats->Defense)));
	Stats->Health -= Damage;

}


