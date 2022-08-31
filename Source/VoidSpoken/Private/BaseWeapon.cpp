// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	WeaponMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Weapon Mesh Component"));
	BoxColliderComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collider Component"));
	WeaponMeshComponent->SetHiddenInGame(false, true);

	RootComponent = WeaponMeshComponent;

	BoxColliderComponent->SetCollisionProfileName(TEXT("Box Trigger"));
	BoxColliderComponent->SetGenerateOverlapEvents(true);
	BoxColliderComponent->SetNotifyRigidBodyCollision(true);
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseWeapon::PostInitializeComponents() {
	Super::PostInitializeComponents();

	BoxColliderComponent->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnComponentBeginOverlap);
	BoxColliderComponent->OnComponentHit.AddDynamic(this, &ABaseWeapon::OnComponentHit);
	BoxColliderComponent->OnComponentEndOverlap.AddDynamic(this, &ABaseWeapon::OnComponentEndOverlap);
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
	return this->ComboAnimationMontage.Max();
}

ACharacter* ABaseWeapon::GetEquippedCharacter() {
	return this->EquippedCharacter;
}

void ABaseWeapon::Equip(ACharacter* EquippingActor) {
	EquippedCharacter = EquippingActor;
	SetOwner(EquippingActor);
	DefaultAnimInstance = EquippingActor->GetMesh()->GetAnimInstance();
	EquippedCharacterMovementComponent = EquippingActor->GetCharacterMovement();
}

void ABaseWeapon::Attack() {
	if (CurrentComboIndex >= GetComboLength()) Reset();

	//Disabling Actors movement while attacking
	EquippedCharacterMovementComponent->SetMovementMode(MOVE_None);

	//Managing the Timers and Delays
	if (GetWorldTimerManager().IsTimerActive(ResetDelay)) GetWorldTimerManager().ClearTimer(ResetDelay);
	if (GetWorldTimerManager().GetTimerRemaining(AttackDelay) <= 0.0f && ComboAnimationMontage.IsValidIndex(CurrentComboIndex)) {
		EquippedCharacter->GetMesh()->PlayAnimation(ComboAnimationMontage[CurrentComboIndex], false);
		GetWorldTimerManager().SetTimer(AttackDelay, this, &ABaseWeapon::NextAttack, ComboAnimationMontage[CurrentComboIndex]->GetPlayLength(), false);
	}
}

void ABaseWeapon::DealDamage() {

}

void ABaseWeapon::NextAttack() {
	//Checking if the Blending Array is valid
	if (ComboBlendingMontage.IsValidIndex(CurrentComboIndex)) {
		EquippedCharacter->GetMesh()->PlayAnimation(ComboBlendingMontage[CurrentComboIndex], false);
		
		//Setting the ResetDelay to become active during the blending animation's length,
		GetWorldTimerManager().SetTimer(ResetDelay, this, &ABaseWeapon::Reset, ComboBlendingMontage[CurrentComboIndex]->GetPlayLength(), false);
	}
	else {
		GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Red, "Blending Combo Sequence Array[" + FString::FromInt(CurrentComboIndex) + "] Not Intialized");
		
		GetWorldTimerManager().SetTimer(ResetDelay, this, &ABaseWeapon::Reset, 2.25f, false);
	}

	//Re-Enabling Actors movement after attacking
	EquippedCharacterMovementComponent->SetMovementMode(MOVE_Walking);

	CurrentComboIndex++;
}

void ABaseWeapon::UniqueAttack() {
	//Disabling Actors movement while attacking
	EquippedCharacterMovementComponent->SetMovementMode(MOVE_None);

	//Vailidating the UniqueAttackSequence
	if (GetWorldTimerManager().GetTimerRemaining(AttackDelay) > 0.0f && UniqueAttackMontage) {
		EquippedCharacter->GetMesh()->PlayAnimation(UniqueAttackMontage, false);
		GetWorldTimerManager().SetTimer(AttackDelay, this, &ABaseWeapon::NextAttack, UniqueAttackMontage->GetPlayLength(), false);
		GetWorldTimerManager().SetTimer(UniqueAttackDelay, this, &ABaseWeapon::Reset, UniqueAttackMontage->GetPlayLength() + UniqueAttackCooldown, false);
	}
}

void ABaseWeapon::Reset() {
	GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Green, "Resetted Combo");
	GetWorldTimerManager().ClearTimer(ResetDelay);
	EquippedCharacter->GetMesh()->AnimScriptInstance = DefaultAnimInstance;
	CurrentComboIndex = 0;

	//Re-Enabling Actors movement just in case the attacks do not reset the characters movement
	EquippedCharacterMovementComponent->SetMovementMode(MOVE_Walking);
	EquippedCharacter->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
}

void ABaseWeapon::OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComponent)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Overlap Begin"));
	}
}

void ABaseWeapon::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	if (OtherActor && (OtherActor != this) && OtherComponent)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Overlap End"));
	}
}

void ABaseWeapon::OnComponentHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) {
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Component Hit"));
}