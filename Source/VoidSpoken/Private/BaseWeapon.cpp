// Fill out your copyright notice in the Description page of Project Settings.

/*
* Script Name:	BaseWeapon.cpp
* Description:
*		Creating a generic weapon class with required functions and variables. Intended for
*	creating sub-classes deriving from this class to have their own unqiue features.
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/

#include "BaseWeapon.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh Component"));
	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Collision Box"));
	WeaponMeshComponent->SetHiddenInGame(false, true);
	RootComponent = WeaponMeshComponent;
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseWeapon::PostInitializeComponents() {
	Super::PostInitializeComponents();

	WeaponCollisionBox->SetGenerateOverlapEvents(true);
	WeaponCollisionBox->SetNotifyRigidBodyCollision(true);

	const FAttachmentTransformRules* TransformRules = new FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, false);
	WeaponCollisionBox->AttachToComponent(WeaponMeshComponent, *TransformRules, "Weapon Collision Box");

	//Weapon Collision Delegates
	WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnComponentBeginOverlap);
	WeaponCollisionBox->OnComponentHit.AddDynamic(this, &ABaseWeapon::OnComponentHit);
	WeaponCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ABaseWeapon::OnComponentEndOverlap);
}

// Called every frame
void ABaseWeapon::Tick(const float DeltaTime) {
	Super::Tick(DeltaTime);
	
	if (bIsAttacking)
		EquippedCharacter->AddActorLocalOffset(FVector(EquippedCharacter->GetMesh()->GetAnimInstance()->GetCurveValue(FName("Movement Delta (Forward)")), 0, 0));
}

void ABaseWeapon::Equip_Implementation(ACharacter* EquippingCharacter) {
	EquippedCharacterMovementComponent = EquippingCharacter->GetCharacterMovement();
	EquippedCharacter = EquippingCharacter;
}

void ABaseWeapon::Attack() {
	if (!bAttackDelay && CheckMovementMode()) {
		if (ComboAnimationMontage.IsValidIndex(CurrentComboIndex) && GetComboLength() > 0) {
			// On Attack Started
			OnAttackStarted.ExecuteIfBound();
			bIsAttacking = true;

			//Check the current index to make sure we do not reference something unwanted
			if (CurrentComboIndex >= GetComboLength()) Reset();

			//Disabling Actors movement while attacking
			EquippedCharacterMovementComponent->SetMovementMode(MOVE_None);

			bAttackDelay = true;
			EAttackState = EAT_NormalAttack;
			EquippedCharacter->GetMesh()->GetAnimInstance()->Montage_Play(ComboAnimationMontage[CurrentComboIndex]);
		}
	}
}

void ABaseWeapon::NextAttack() {
	// On Attack Ended
	OnAttackEnded.ExecuteIfBound();

	bAttackDelay = false;
	EAttackState = EAT_None;
	OverlappedActors.Empty();
	EquippedCharacterMovementComponent->SetMovementMode(MOVE_None);
	CurrentComboIndex++;
}

void ABaseWeapon::DealDamage(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent) {
	float DamageMultiplier;
	switch (EAttackState) {
	case (EAT_NormalAttack):
		if (ComboAttackMultipliers.IsValidIndex(CurrentComboIndex))
			DamageMultiplier = ComboAttackMultipliers[CurrentComboIndex];
		else DamageMultiplier = 1;
		break;
	case (EAT_ChargedAttack):
		DamageMultiplier = ChargedAttackComboMultiplier;
		break;
	default:
		DamageMultiplier = 1.0f;
	}
	
	UGameplayStatics::ApplyDamage(OtherActor, BaseDamage * DamageMultiplier, NULL, EquippedCharacter, NULL);
}

bool ABaseWeapon::CheckMovementMode() const {
	switch (EquippedCharacterMovementComponent->MovementMode) {
	case (MOVE_None):
	case (MOVE_Walking):
			return true;
	default: 
		return false;
	}
}

void ABaseWeapon::Clear() {
	OverlappedActors.Empty();
}

void ABaseWeapon::Reset() {
	bIsAttacking = false;
	EAttackState = EAT_None;
	CurrentComboIndex = 0;
	OverlappedActors.Empty();

	//Re-Enabling Actors movement just in case the attacks do not reset the characters movement
	EquippedCharacterMovementComponent->SetMovementMode(MOVE_Walking);
}

#pragma region Deprecated Functions / To be implemented

[[deprecated]] void ABaseWeapon::ChargedAttack()
{
	if (!bAttackDelay && !bIsAttacking && CheckMovementMode()) {
		if (ChargedAttackMontage && EquippedCharacter->FindComponentByClass<UStatsMasterClass>()->Stamina >= ChargedAttackConsumption) {
			bIsAttacking = true;

			//Disabling Actors movement while attacking
			EquippedCharacterMovementComponent->SetMovementMode(MOVE_None);

			EAttackState = EAT_ChargedAttack;
			EquippedCharacter->GetMesh()->GetAnimInstance()->Montage_Play(ComboAnimationMontage[CurrentComboIndex]);
		}
	}
}

#pragma endregion

#pragma region Collision Functions

void ABaseWeapon::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsAttacking && OtherActor && (OtherActor != this) && (OtherActor != EquippedCharacter) && OtherComponent) {
		if (!OverlappedActors.Contains(OtherActor)) {
			DealDamage(OverlappedComponent, OtherActor, OtherComponent);
			OverlappedActors.AddUnique(OtherActor);
		}
	}
}

[[deprecated]] void ABaseWeapon::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (bIsAttacking && OtherActor && (OtherActor != this) && (OtherActor != EquippedCharacter) && OtherComponent) {
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Overlap End: " + OtherActor->GetName()));
	}
}

/// May not be used later on since these weapon will work off of overlapping actors instead of hitting and moving other actor around.
[[deprecated]] void ABaseWeapon::OnComponentHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) {
	if (bIsAttacking && OtherActor && (OtherActor != this) && (OtherActor != EquippedCharacter) && OtherComponent) {
		DealDamage(OverlappedComponent, OtherActor, OtherComponent);
	}
}

#pragma endregion