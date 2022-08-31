// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "BaseWeapon.generated.h"

UCLASS(Abstract)
class VOIDSPOKEN_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	//Getter Statements

	/// Function: GetBaseDamage()
	///		-Returns the float BaseDamage
	UFUNCTION(BlueprintPure, BlueprintCallable)
		float GetBaseDamage();

	/// Function: GetCurrentComboIndex()
	///		-Returns the int CurrentComboIndex
	UFUNCTION(BlueprintPure, BlueprintCallable)
		int GetCurrentComboIndex();

	/// Function: GetComboLength()
	///		-Returns an int of the Max() of Elements inside the ComboAnimationMontage
	UFUNCTION(BlueprintPure, BlueprintCallable)
		int GetComboLength();

	/// Function: GetEquippedCharacter() 
	///		-Returns a ACharacter* of the variable EquippedCharacter
	UFUNCTION(BlueprintPure, BlueprintCallable)
		ACharacter* GetEquippedCharacter();

	//Weapon Functions

	/// Function: Equip(ACharacter* EquippingCharacter) 
	///		-This functions sets the variable EquippedCharacter
	///		-Will Also set this Weapon's owner as EquippedCharacter
	///		-Initializes the UAnimInstance from the Default 
	///			*(At the time of this Equip(), what was the UAnimInstance the Character had)
	///		-Initializes the UCharacterMovementComponent from the given EquippedCharacter 
	UFUNCTION(BlueprintCallable)
		void Equip(ACharacter* EquippingCharacter);

	/// Function: Attack()
	///		-Momentarly disables player movement
	///		-Set Timers (AttackDelay) to be active and delays for the respective UAnimMontage's PlayLength()
	///		-Will not play any animations if the ComboAnimationMontage are not set 
	UFUNCTION(BlueprintCallable)
		void Attack();

	/// Function: DealDamage()
	///		-Checks if the Attached Box Collider is overlapping any Actors (Preferably if their from a BaseEntity class)
	///		-Length of this function is determined in each of the ComboAnimationMontage
	///		-Uses the Notify and NotifyState systems in each Animation
	UFUNCTION(BlueprintCallable)
		void DealDamage();

	/// Function: NextAttack()
	///		-Re-Enables the player movement
	///		-Sets another Timer (ResetDelay), which calls Reset()
	///		-Itterates the CurrentComboCount
	UFUNCTION(BlueprintCallable)
		void NextAttack();

	/// Function: UniqueAttack()
	///		-Disables the player movement
	///		-Plays the UniqueAnimationMontage
	///		-After UniqueAnimationMontage is done, then Plays the UniqueBlendingMontage
	///		-Sets Timer (NextAttack)
	///		-Sets Timer (UniqueAttackDelay)
	UFUNCTION(BlueprintCallable)
		void UniqueAttack();

	/// Function: Reset() 
	///		-Resets the player back into their Default Animation Instance
	///		-Sets CurrentComboIndex to 0
	///		-Enables Player Movement
	///		-Clears Timers (AttackDelay, ResetDelay)
	///	*NOTES*
	///		Only Use when the player either has been inturupted (via rolling, falling, stunned, etc)
	UFUNCTION(BlueprintCallable)
		void Reset();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Weapon Parameters

	/// Base Damage of this Weapon
	///		-Any scaling involving the damage of the Attack, this value will influence
	///		-Can be set to any float value
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (NoGetter))
		float BaseDamage = 0;

	/// The Actor that has equipped this weapon
	///		-For purposes of unwanted self damage infliction
	///		-This will also be used to equip to this Actor
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		ACharacter* EquippedCharacter;

	/// This Array contains all the Animation Montages for the Attack of this weapon.
	///		-This cannot be unintialized
	///		-This will loop constantly when this runs through the whole Montage
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TArray<UAnimMontage*> ComboAnimationMontage = {};

	/// Contains all the Animations for the Attack(s) after being played.
	///		-These are in their respective order with the ComboMontage Elements
	///		-These will play after the Attack Animation has been finished
	///		-Will be interrupted when executing another attack 
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TArray<UAnimMontage*> ComboBlendingMontage = {};
	
	/// This keeps track of the current Attack to be executed.
	///		-Cannot be edited by blueprints to prevent any unwanted behaviours
	///		-Will only increment to the length of the ComboMontage minus 1
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (NoGetter))
		int CurrentComboIndex = 0;

	//Unique Attack
	
	/// This Unique Animation will be played if given the opportunity
	///		-Given at the desired index, will interrupt the current attacks animation
	///		-This parameter can be Uninitalized (not all weapons need a unique attack)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UAnimMontage* UniqueAttackMontage = nullptr;

	/// This Unique Blending Animation will be played after the Animation
	///		-Only Plays if the UniqueAttackMontage is also valid
	///		-Only Plays after the UniqueAttackMontage
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UAnimMontage* UniqueBlendingMontage = nullptr;

	/// Where will this window of opporunity be opened at
	///		-This can happen anywhere from 0 to ComboMontage - 1.
	///		-Will not play if UniqueAnimationMontage is null, or unintialized
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int UniqueAttackIndex = 0;

	/// The cooldown (in seconds) after the attack has been initiated
	///		-This is additive of the UniqueAttackMontage's play length (in seconds)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float UniqueAttackCooldown = 0;

	/// Components of this class

	/// Box Collider
	///		-This collider will always Ignore the Owning Actor/Character
	///		-The Bounds of this box must be set in-engine
	UPROPERTY(EditDefaultsOnly)
		class UBoxComponent* BoxColliderComponent = nullptr;
	
	/// Mesh of this weapon
	///		-Can be uninitialized, although not recommended
	///		-Collision will be based off of this mesh (Simple Collision)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* WeaponMeshComponent;

	/// Actors UAnimMontage or UAnimBlueprint after they've been reset (Bringing them to their default state)
	UPROPERTY(BlueprintReadOnly)
		class UAnimInstance* DefaultAnimInstance;
	
	//Equipped Actors movement component to be set disabled and re-enabled when attacking
	UPROPERTY(BlueprintReadOnly)
		class UCharacterMovementComponent* EquippedCharacterMovementComponent;

	//Timer Handles

	//Timer Handler for NextAttack()
	FTimerHandle AttackDelay;

	//Timer Handler for UniqueAttack()
	FTimerHandle UniqueAttackDelay;

	//Timer Handler for Reset()
	FTimerHandle ResetDelay;

	//Collision Delegates
	UFUNCTION()
		void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
		void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);
	
	UFUNCTION()
		void OnComponentHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
