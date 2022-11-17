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

	FAttachmentTransformRules* TransformRules = new FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
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

float ABaseWeapon::GetBaseDamage() {
	return BaseDamage;
}

int ABaseWeapon::GetCurrentComboIndex() {
	return CurrentComboIndex;
}

int ABaseWeapon::GetComboLength() {
	return ComboAnimationSequence.Max();
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

void ABaseWeapon::Equip(ACharacter* EquippingCharacter) {
	EquippedCharacter = EquippingCharacter;
	SetOwner(EquippingCharacter);
	DefaultAnimInstance = EquippingCharacter->GetMesh()->GetAnimInstance();
	EquippedCharacterMovementComponent = EquippingCharacter->GetCharacterMovement();	
}

void ABaseWeapon::Attack() {
	if (!AttackDelay && !IsAttacking && CheckMovementMode()) {
		if (GetComboLength() > 0) {
			IsAttacking = true;

			//Check the current index to make sure we do not reference something unwanted
			if (CurrentComboIndex >= GetComboLength()) Reset();

			//Disabling Actors movement while attacking
			EquippedCharacterMovementComponent->SetMovementMode(MOVE_None);

			if (ComboAnimationSequence.IsValidIndex(CurrentComboIndex)) EquippedCharacter->GetMesh()->PlayAnimation(ComboAnimationSequence[CurrentComboIndex], false);
		}
	}
}

[[deprecated]] void ABaseWeapon::DealDamage() {

}

void ABaseWeapon::NextAttack() {
	//Re-Enabling Actors movement after attacking
	IsAttacking = false;
	EquippedCharacterMovementComponent->SetMovementMode(MOVE_None);
	CurrentComboIndex++;
}


[[deprecated]] void ABaseWeapon::UniqueAttack() {
	//Vailidating the UniqueAttackSequence
	if (!AttackDelay && UniqueAttackSequence) {
		//Disabling Actors movement while attacking
		EquippedCharacterMovementComponent->SetMovementMode(MOVE_None);

		EquippedCharacter->GetMesh()->PlayAnimation(UniqueAttackSequence, false);
		GetWorldTimerManager().SetTimer(UniqueAttackDelayTimer, this, &ABaseWeapon::Reset, UniqueAttackSequence->GetPlayLength() + UniqueAttackCooldown, false);
	}
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
	EquippedCharacter->GetMesh()->AnimScriptInstance = DefaultAnimInstance;
	CurrentComboIndex = 0;

	//Re-Enabling Actors movement just in case the attacks do not reset the characters movement
	EquippedCharacterMovementComponent->SetMovementMode(MOVE_Walking);
	EquippedCharacter->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
}

[[deprecated]] void ABaseWeapon::UniqueReset() {
	GetWorldTimerManager().ClearTimer(UniqueAttackDelayTimer);
	EquippedCharacter->GetMesh()->AnimScriptInstance = DefaultAnimInstance;
}

void ABaseWeapon::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsAttacking && OtherActor && (OtherActor != this) && (OtherActor != EquippedCharacter) && OtherComponent)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Overlap Begin: " + OtherActor->GetName()));
		UGameplayStatics::ApplyDamage(OtherActor, BaseDamage, NULL, EquippedCharacter, NULL);
	}
}

void ABaseWeapon::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (IsAttacking && OtherActor && (OtherActor != this) && (OtherActor != EquippedCharacter) && OtherComponent)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Overlap End: " + OtherActor->GetName()));
	}
}

void ABaseWeapon::OnComponentHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) {
	if (IsAttacking && OtherActor && (OtherActor != this) && (OtherActor != EquippedCharacter) && OtherComponent)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Hit: " + OtherActor->GetName()));
		UGameplayStatics::ApplyDamage(OtherActor, BaseDamage, NULL, EquippedCharacter, NULL);
	}
}