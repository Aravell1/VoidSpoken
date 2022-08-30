// Fill out your copyright notice in the Description page of Project Settings.


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

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float ABaseWeapon::GetBaseDamage() {
	return this->BaseDamage;
}

int ABaseWeapon::GetCurrentComboIndex() {
	return this->CurrentComboIndex;
}

int ABaseWeapon::GetComboLength() {
	return this->ComboMontage.Max();
}

void ABaseWeapon::Equip(ACharacter* EquippingActor) {
	EquippedActor = EquippingActor;
}

void ABaseWeapon::Attack() {
	if (CurrentComboIndex >= GetComboLength()) Reset();

	if (GetWorldTimerManager().IsTimerActive(ResetDelay)) GetWorldTimerManager().ClearTimer(ResetDelay);
	if (GetWorldTimerManager().GetTimerRemaining(AttackDelay) <= 0.0f) {
		EquippedActor->GetMesh()->PlayAnimation(ComboMontage[CurrentComboIndex], false);
		GetWorldTimerManager().SetTimer(AttackDelay, this, &ABaseWeapon::NextAttack, ComboMontage[CurrentComboIndex]->GetPlayLength(), false);
	}
}

void ABaseWeapon::NextAttack() {
	//Checking if the Blending Array is valid
	if (ComboBlendingMontage.IsValidIndex(CurrentComboIndex)) {
		EquippedActor->GetMesh()->PlayAnimation(ComboBlendingMontage[CurrentComboIndex], false);
		GetWorldTimerManager().SetTimer(ResetDelay, this, &ABaseWeapon::Reset, ComboBlendingMontage[CurrentComboIndex]->GetPlayLength(), false);
	}
	else {
		GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Red, "Blending Combo Montage Array[" + FString::FromInt(CurrentComboIndex) + "] Not Intialized");
		
		//Setting the ResetDelay to become active during the blending animation's length,
		GetWorldTimerManager().SetTimer(ResetDelay, this, &ABaseWeapon::Reset, 1.25f, false);
	}

	CurrentComboIndex++;
}

void ABaseWeapon::UniqueAttack() {
	//Vailidating the UniqueAttackSequence
	if (GetWorldTimerManager().GetTimerRemaining(AttackDelay) > 0.0f && UniqueAttackMontage) {
		EquippedActor->GetMesh()->PlayAnimation(UniqueAttackMontage, false);
		GetWorldTimerManager().SetTimer(AttackDelay, this, &ABaseWeapon::NextAttack, UniqueAttackMontage->GetPlayLength(), false);
	}

	Reset();
}

void ABaseWeapon::Reset() {
	GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Green, "Resetted Combo");
	GetWorldTimerManager().ClearTimer(AttackDelay);
	CurrentComboIndex = 0;
}
