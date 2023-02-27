// Fill out your copyright notice in the Description page of Project Settings.


#include "SpikeProjectile.h"

// Sets default values
ASpikeProjectile::ASpikeProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	if (!CollisionComponent)
	{
		CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
		CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("Projectile"));
		CollisionComponent->OnComponentHit.AddDynamic(this, &ASpikeProjectile::OnHit);
		CollisionComponent->InitSphereRadius(15.0f);
		RootComponent = CollisionComponent;
	}

	if (!ProjectileMovementComponent)
	{
		ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
		ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
		ProjectileMovementComponent->InitialSpeed = 1500;
		ProjectileMovementComponent->MaxSpeed = 1500;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
	}

	if (!ProjectileMeshComponent)
	{
		ProjectileMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMeshComponent"));
		ProjectileMeshComponent->SetupAttachment(CollisionComponent);
	}
}

void ASpikeProjectile::SetDamage(float NewDamage, bool Collision, float LifeTime)
{
	Damage = NewDamage;
	UseCollision = Collision;
	SetLifeSpan(LifeTime);
}

void ASpikeProjectile::SetSpeed(float InitialSpeed)
{
	ProjectileMovementComponent->MaxSpeed = InitialSpeed * 2;
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->Velocity = GetActorForwardVector() * ProjectileMovementComponent->InitialSpeed;
}

// Called when the game starts or when spawned
void ASpikeProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ASpikeProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor != GetOwner())
	{
		if (UseCollision)
		{
			if (OtherActor == Cast<AActor>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
			{
				UGameplayStatics::ApplyDamage(OtherActor, Damage, NULL, this, NULL);
			}
		}
		Destroy();
	}
}

// Called every frame
void ASpikeProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

