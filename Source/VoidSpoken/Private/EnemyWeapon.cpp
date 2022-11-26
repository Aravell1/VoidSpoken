// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyWeapon.h"

AEnemyWeapon::AEnemyWeapon()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh Component"));
	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Collision Box"));
	WeaponMeshComponent->SetHiddenInGame(false, true);
	RootComponent = WeaponMeshComponent;
}

void AEnemyWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	WeaponCollisionBox->SetGenerateOverlapEvents(true);
	WeaponCollisionBox->SetNotifyRigidBodyCollision(true);

	FAttachmentTransformRules* TransformRules = new FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, false);
	WeaponCollisionBox->AttachToComponent(WeaponMeshComponent, *TransformRules, "Weapon Collision Box");

	//Weapon Collision Delegates
	WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEnemyWeapon::OnComponentBeginOverlap);
}

float AEnemyWeapon::GetBaseDamage()
{
	return BaseDamage;
}

void AEnemyWeapon::SetBaseDamage(float Damage)
{
	BaseDamage = Damage;
}

ACharacter* AEnemyWeapon::GetEquippedCharacter()
{
	return EquippedCharacter;
}

void AEnemyWeapon::Equip(ACharacter* EquippingCharacter, float Damage)
{
	EquippedCharacter = EquippingCharacter;
	SetOwner(EquippingCharacter);
	SetBaseDamage(Damage);
}

void AEnemyWeapon::DealDamage(AActor* OtherActor)
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Purple, FString::SanitizeFloat(BaseDamage));
	UGameplayStatics::ApplyDamage(OtherActor, BaseDamage, NULL, EquippedCharacter, NULL);
}

void AEnemyWeapon::SetIsAttacking(bool IsAttacking)
{
	Attacking = IsAttacking;
}

void AEnemyWeapon::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Attacking && OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
	{
		DealDamage(OtherActor);
	}
}

