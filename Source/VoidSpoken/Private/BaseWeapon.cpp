// Fill out your copyright notice in the Description page of Project Settings.

/*
* Script Name:	BaseWeapon.cpp
* Description:
*		Creating a generic weapon class with required functions and variables. Intended for
*	creating sub-classes deriving from this class to have their own unique features.
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

	WeaponMaterialInterface = WeaponMeshComponent->GetMaterial(0);
}

void ABaseWeapon::PostInitializeComponents() {
	Super::PostInitializeComponents();

	WeaponCollisionBox->SetGenerateOverlapEvents(true);
	WeaponCollisionBox->SetNotifyRigidBodyCollision(true);

	const FAttachmentTransformRules* TransformRules = new FAttachmentTransformRules(EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, EAttachmentRule::KeepRelative, false);
	WeaponCollisionBox->AttachToComponent(WeaponMeshComponent, *TransformRules, "Weapon Collision Box");

	//Weapon Collision Delegates
	WeaponCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnComponentBeginOverlap);
}

// Called every frame
void ABaseWeapon::Tick(const float DeltaTime) {
	Super::Tick(DeltaTime);

	// Change for player based input, still moving into walls and such
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, "Equipped Character Forward: " + EquippedCharacter->GetActorForwardVector().ToString() + " Forward Delta: " + FString::SanitizeFloat(EquippedCharacter->GetMesh()->GetAnimInstance()->GetCurveValue(FName("Movement Delta (Forward)"))));
	EquippedCharacter->AddActorLocalOffset(FVector(EquippedCharacter->GetMesh()->GetAnimInstance()->GetCurveValue(FName("Movement Delta (Forward)")), 0, 0));
	//EquippedCharacter->AddMovementInput(EquippedCharacter->GetActorForwardVector(), EquippedCharacter->GetMesh()->GetAnimInstance()->GetCurveValue(FName("Movement Delta (Forward)")));
}

void ABaseWeapon::Equip_Implementation(ACharacter* EquippingCharacter, FName EquippingSlot = FName("Left")) {
	if (EquippingSlot == "Left") EEquippedSlot = EEquippedSlot::EES_Left;
	else if (EquippingSlot == "Right") EEquippedSlot = EEquippedSlot::EES_Right;
	
	EquippedCharacterMovementComponent = EquippingCharacter->GetCharacterMovement();
	EquippedCharacter = EquippingCharacter;
	SetOwner(EquippingCharacter);

	WeaponMeshComponent->AddLocalOffset(LocalWeaponLocationOffset);
	WeaponMeshComponent->AddLocalRotation(LocalWeaponRotationOffset);
}

void ABaseWeapon::Attack() {
	if (ComboAnimationMontage.IsValidIndex(CurrentComboIndex) && GetComboLength() > 0) {
		if (EquippedCharacter->GetMesh()->GetAnimInstance()->GetCurrentActiveMontage() != ComboAnimationMontage[CurrentComboIndex] && !bAttackDelay && CheckMovementMode()) {
			// On Attack Started
			OnAttackStarted.ExecuteIfBound();

			//Check the current index to make sure we do not reference something unwanted
			if (CurrentComboIndex >= GetComboLength()) Reset();

			//Disabling Actors movement while attacking
			EquippedCharacterMovementComponent->SetMovementMode(MOVE_None);

			EquippedCharacter->GetMesh()->GetAnimInstance()->Montage_Play(ComboAnimationMontage[CurrentComboIndex]);
		}
	}
}

void ABaseWeapon::NextAttack() {
	// On Attack Ended
	OnAttackEnded.ExecuteIfBound();
	CurrentComboIndex++;
	bAttackDelay = false;
	OverlappedActors.Empty();
	EquippedCharacterMovementComponent->SetMovementMode(MOVE_None);
}

void ABaseWeapon::DealDamage(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent) {
	float DamageMultiplier;
	if (ComboAttackMultipliers.IsValidIndex(CurrentComboIndex))
		DamageMultiplier = ComboAttackMultipliers[CurrentComboIndex];
	else DamageMultiplier = 1;
	UGameplayStatics::ApplyDamage(OtherActor, BaseDamage * DamageMultiplier, NULL, EquippedCharacter, UDamageTypeStagger::StaticClass());
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
	CurrentComboIndex = 0;
	OverlappedActors.Empty();

	//Re-Enabling Actors movement just in case the attacks do not reset the characters movement
	EquippedCharacterMovementComponent->SetMovementMode(MOVE_Walking);
}

#pragma region Collision Functions

void ABaseWeapon::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCheckForOverlappedActors && OtherActor && (OtherActor != this) && (OtherActor != EquippedCharacter) && OtherComponent) {
		if (!OverlappedActors.Contains(OtherActor)) {
			DealDamage(OverlappedComponent, OtherActor, OtherComponent);
			OverlappedActors.AddUnique(OtherActor);
		}
	}
}

#pragma endregion