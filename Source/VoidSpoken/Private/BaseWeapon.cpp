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
	if (!AttackDelay) {
		//Check the current index to make sure we do not reference something unwanted
		if (CurrentComboIndex >= GetComboLength()) Reset();

		//Disabling Actors movement while attacking
		EquippedCharacterMovementComponent->SetMovementMode(MOVE_None);

		if (ComboAnimationSequence.IsValidIndex(CurrentComboIndex)) EquippedCharacter->GetMesh()->PlayAnimation(ComboAnimationSequence[CurrentComboIndex], false);
	}
}

void ABaseWeapon::DealDamage() {
	/// Preinitialize Variables 
	TArray<FHitResult> OutHits;
	FVector ActorLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	FCollisionShape CollisionBox = FCollisionShape::MakeSphere(250.f);

	//DrawDebugSphere(GetWorld(), ActorLocation, CollisionBox.GetSphereRadius(), 100, FColor::Purple, true);

	bool IsHit = GetWorld()->SweepMultiByChannel(OutHits, ActorLocation, ActorLocation, FQuat::Identity, ECC_WorldStatic, CollisionBox);

	if (IsHit) {
		for (auto& Hit : OutHits) {
			if (*Hit.GetActor()->GetName() != GetName() && *Hit.GetActor()->GetName() != EquippedCharacter->GetName()) {
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Hit Result: %s"), *Hit.GetActor()->GetName()));
			}
		}
	}


}

void ABaseWeapon::NextAttack() {
	//Re-Enabling Actors movement after attacking
	EquippedCharacterMovementComponent->SetMovementMode(MOVE_Walking);
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

void ABaseWeapon::Reset() {
	SetAttackDelay(false);
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