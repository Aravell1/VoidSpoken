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
	PrimaryActorTick.bCanEverTick = false;
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

	FAttachmentTransformRules* TransformRules = new FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, false);
	WeaponCollisionBox->AttachToComponent(WeaponMeshComponent, *TransformRules, "Weapon Collision Box");

	//Weapon Collision Delegates
	WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnComponentBeginOverlap);
	WeaponCollisionBox->OnComponentHit.AddDynamic(this, &ABaseWeapon::OnComponentHit);
	WeaponCollisionBox->OnComponentEndOverlap.AddDynamic(this, &ABaseWeapon::OnComponentEndOverlap);
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

#pragma region Getter and Setter

float ABaseWeapon::GetBaseDamage() {
	return BaseDamage;
}

int ABaseWeapon::GetCurrentComboIndex() {
	return CurrentComboIndex;
}

int ABaseWeapon::GetComboLength() {
	return ComboAnimationMontage.Max();
}

ACharacter* ABaseWeapon::GetEquippedCharacter() {
	return EquippedCharacter;
}

bool ABaseWeapon::GetAttackDelay() {
	return AttackDelay;
}

void ABaseWeapon::SetAttackDelay(const bool state) {
	AttackDelay = state;
}

bool ABaseWeapon::GetCanUniqueAttack() {
	return CanUniqueAttack;
}

void ABaseWeapon::SetCanUniqueAttack(const bool state) {
	CanUniqueAttack = state;
}

#pragma endregion

void ABaseWeapon::Equip_Implementation(ACharacter* EquippingCharacter) {
	EquippedCharacterMovementComponent = EquippingCharacter->GetCharacterMovement();
	EquippedCharacter = EquippingCharacter;
}

void ABaseWeapon::Attack() {
	if (!AttackDelay && !IsAttacking && CheckMovementMode()) {
		// On Attack Started
		OnAttackStarted.ExecuteIfBound();

		if (GetComboLength() > 0) {
			IsAttacking = true;

			//Check the current index to make sure we do not reference something unwanted
			if (CurrentComboIndex >= GetComboLength()) Reset();

			//Disabling Actors movement while attacking
			EquippedCharacterMovementComponent->SetMovementMode(MOVE_None);

			AttackDelay = true;
			AttackState = AttackType::NormalAttack;
			if (ComboAnimationMontage.IsValidIndex(CurrentComboIndex)) EquippedCharacter->GetMesh()->GetAnimInstance()->Montage_Play(ComboAnimationMontage[CurrentComboIndex]);
		}
	}
}

void ABaseWeapon::NextAttack() {
	// On Attack Ended
	OnAttackEnded.ExecuteIfBound();

	IsAttacking = false;
	AttackState = AttackType::None;
	OverlappedActors.Empty();
	EquippedCharacterMovementComponent->SetMovementMode(MOVE_None);
	CurrentComboIndex++;
}

void ABaseWeapon::DealDamage(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent) {
	float DamageMultipiler = 0;
	switch (AttackState) {
	case (AttackType::NormalAttack):
		if (ComboAttackMultipliers.IsValidIndex(CurrentComboIndex))
			DamageMultipiler = ComboAttackMultipliers[CurrentComboIndex];
		else DamageMultipiler = 1;
		break;
	case (AttackType::ChargedAttack):
		DamageMultipiler = ChargedAttackComboMultiplier;
		break;
	case (AttackType::UniqueAttack):
		DamageMultipiler = UniqueAttackComboMultiplier;
		break;
	}
	if (DamageMultipiler > 0)
	UGameplayStatics::ApplyDamage(OtherActor, BaseDamage * DamageMultipiler, NULL, EquippedCharacter, UDamageTypeStagger::StaticClass());
}

bool ABaseWeapon::CheckMovementMode() {
	switch (EquippedCharacterMovementComponent->MovementMode) {
	case (MOVE_None):
	case (MOVE_Walking):
			return true;
	default: 
		return false;
	}
}

void ABaseWeapon::Reset() {
	SetAttackDelay(false);
	IsAttacking = false;
	AttackState = AttackType::None;
	CurrentComboIndex = 0;

	OverlappedActors.Empty();

	//Re-Enabling Actors movement just in case the attacks do not reset the characters movement
	EquippedCharacterMovementComponent->SetMovementMode(MOVE_Walking);
}

#pragma region Deprecated Functions / To be implemented

[[deprecated]] void ABaseWeapon::ChargedAttack()
{
	if (!AttackDelay && !IsAttacking && CheckMovementMode()) {
		if (ChargedAttackMontage && EquippedCharacter->FindComponentByClass<UStatsMasterClass>()->Stamina >= ChargedAttackConsumption) {
			IsAttacking = true;

			//Disabling Actors movement while attacking
			EquippedCharacterMovementComponent->SetMovementMode(MOVE_None);

			AttackState = AttackType::ChargedAttack;
			EquippedCharacter->GetMesh()->GetAnimInstance()->Montage_Play(ComboAnimationMontage[CurrentComboIndex]);
		}
	}
}

[[deprecated]] void ABaseWeapon::UniqueAttack() {
	//Vailidating the UniqueAttackMontage
	if (!AttackDelay && UniqueAttackMontage) {
		//Disabling Actors movement while attacking
		EquippedCharacterMovementComponent->SetMovementMode(MOVE_None);

		EquippedCharacter->GetMesh()->GetAnimInstance()->Montage_Play(ComboAnimationMontage[CurrentComboIndex]);
		GetWorldTimerManager().SetTimer(UniqueAttackDelayTimer, this, &ABaseWeapon::Reset, UniqueAttackMontage->GetPlayLength() + UniqueAttackCooldown, false);
	}
}

[[deprecated]] void ABaseWeapon::UniqueReset() {
	GetWorldTimerManager().ClearTimer(UniqueAttackDelayTimer);
}

#pragma endregion

#pragma region Collision Functions

void ABaseWeapon::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsAttacking && OtherActor && (OtherActor != this) && (OtherActor != EquippedCharacter) && OtherComponent)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Overlap Begin: " + OtherActor->GetName()));
		if (!OverlappedActors.Contains(OtherActor)) {
			DealDamage(OverlappedComponent, OtherActor, OtherComponent);
			OverlappedActors.AddUnique(OtherActor);
		}
	}
}

void ABaseWeapon::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (IsAttacking && OtherActor && (OtherActor != this) && (OtherActor != EquippedCharacter) && OtherComponent)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Overlap End: " + OtherActor->GetName()));
	}
}

/// May not be used later on since these weapon will work off of overlapping actors instead of hitting and moving other actor around.
[[deprecated]] void ABaseWeapon::OnComponentHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) {
	if (IsAttacking && OtherActor && (OtherActor != this) && (OtherActor != EquippedCharacter) && OtherComponent)
	{
		#if WITH_ENGINE
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hit: " + OtherActor->GetName()));
		#endif // WITH_ENGINE
		DealDamage(OverlappedComponent, OtherActor, OtherComponent);
	}
}

#pragma endregion