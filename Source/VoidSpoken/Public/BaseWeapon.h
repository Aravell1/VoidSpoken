// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Animation/AnimSequence.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "BaseWeapon.generated.h"

/*
* Script Name:	BaseWeapon.h
* Description:
*		Creating a generic weapon class with required functions and variables. Intended for 
*	creating sub-classes deriving from this class to have their own unqiue features.
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/

UCLASS(Abstract)
class VOIDSPOKEN_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	/// Sets default values for this actor's properties
	ABaseWeapon();

	/// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	///Getter Statements

	/// Function: GetBaseDamage()
	///		-Returns the float BaseDamage
	UFUNCTION(BlueprintPure, BlueprintCallable)
		inline float GetBaseDamage();

	/// Function: GetCurrentComboIndex()
	///		-Returns the int CurrentComboIndex
	UFUNCTION(BlueprintPure, BlueprintCallable)
		inline int GetCurrentComboIndex();

	/// Function: GetComboLength()
	///		-Returns an int of the Max() of Elements inside the ComboAnimationSequence
	UFUNCTION(BlueprintPure, BlueprintCallable)
		inline int GetComboLength();

	/// Function: GetEquippedCharacter() 
	///		-Returns a ACharacter* of the variable EquippedCharacter
	UFUNCTION(BlueprintPure, BlueprintCallable)
		inline ACharacter* GetEquippedCharacter();
	
	/// Function: GetAttackDelay()
	///		-Returns the bool AttackDelay
	UFUNCTION(BlueprintPure, BlueprintCallable)
		inline bool GetAttackDelay();

	/// Function: SetAttackDelay()
	///		-Sets the boolean AttackDelay to {state}
	UFUNCTION(BlueprintCallable)
		inline void SetAttackDelay(const bool state);

	/// Function: GetAttackDelay()
	///		-Returns the boolean CanUniqueAttack
	UFUNCTION(BlueprintPure, BlueprintCallable)
		inline bool GetCanUniqueAttack();

	/// Function: SetCanUniqueAttack()
	///		-Sets the boolean CanUniqueAttack to {state}
	UFUNCTION(BlueprintCallable)
		inline void SetCanUniqueAttack(const bool state);

	///Weapon Functions

	/// Function: Equip(ACharacter* EquippingCharacter) 
	///		-This functions sets the variable EquippedCharacter
	///		-Will Also set this Weapon's owner as EquippedCharacter
	///		-Initialises the UAnimInstance from the Default (At the time of this Equip(), what was the UAnimInstance the Character had)
	///		-Initialises the UCharacterMovementComponent from the given EquippedCharacter 
	UFUNCTION(BlueprintCallable)
		virtual void Equip(ACharacter* EquippingCharacter);

	/// Function: Attack()
	///		-Momentarily disables player movement
	///		-Set Timers (AttackDelay) to be active and delays for the respective UAnimSequence's PlayLength()
	///		-Will not play any animations if the ComboAnimationSequence are not set 
	UFUNCTION(BlueprintCallable)
		virtual void Attack();

	/// Function: DealDamage()
	///		-Checks if the Attached Box Collider is overlapping any Actors (Preferably if their from a BaseEntity class)
	///		-Length of this function is determined in each of the ComboAnimationSequence
	///		-Uses the Notify and NotifyState systems in each Animation
	UFUNCTION(BlueprintCallable)
		virtual void DealDamage();

	/// Function: NextAttack()
	///		-Re-Enables the player movement
	///		-Gets called after Weapon Attack Notify (On End())
	///		-Iterates the CurrentComboCount
	UFUNCTION(BlueprintCallable)
		virtual void NextAttack();

	/// Function: UniqueAttack()
	///		-Disables the player movement
	///		-Plays the UniqueAnimationSequence
	///		-After UniqueAnimationSequence is done, then Plays the UniqueBlendingSequence
	///		-Sets Timer (UniqueAttackDelay)
	UFUNCTION(BlueprintCallable)
		virtual void UniqueAttack();

	/// Function: Reset() 
	///		-Resets the player back into their Default Animation Instance
	///		-Sets CurrentComboIndex to 0
	///		-Enables Player Movement
	///		-Clears Timers (AttackDelay, ResetDelay)
	///		-Only Use when the player either has been interrupted (via rolling, falling, stunned, etc)
	UFUNCTION(BlueprintCallable)
		virtual void Reset();

	/// Function: UniqueReset()
	///		-Gets called after the timer UniqueAttackDelayTimer has expired.
	///		-Sets the UniqueAttackDelay bool to false
	UFUNCTION(BlueprintCallable)
		virtual void UniqueReset();

protected:
	/// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Weapon Parameters

	/// Base Damage of this Weapon
	///		-Any scaling involving the damage of the Attack, this value will influence
	///		-Can be set to any float value (although not negative, may produce unwanted results)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (NoGetter))
		float BaseDamage = 0;

	/// The Actor that has equipped this weapon
	///		-For purposes of unwanted self damage infliction
	///		-This will also be used to equip to this Actor
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		ACharacter* EquippedCharacter;

	/// This Array contains all the Animation Sequences for the Attack of this weapon.
	///		-This cannot be uninitialized
	///		-This will loop constantly when this runs through the whole Sequence Array
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TArray<UAnimSequence*> ComboAnimationSequence = {};

	/// This array will dictate how much of the Base Damage is accounted for
	///		-Normalized Value (%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TArray<float> ComboAttackMultipliers = {};
	
	/// This keeps track of the current Attack to be executed.
	///		-Cannot be edited by blueprints to prevent any unwanted behaviours
	///		-Will only increment to the length of the ComboSequence minus 1
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (NoGetter))
		int CurrentComboIndex = 0;

	//Unique Attack
	
	/// This Unique Animation will be played if given the opportunity
	///		-Given at the desired index (Unique Attack Index), will interrupt the current attacks animation
	///		-This parameter can be uninitialized (not all weapons need a unique attack)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UAnimSequence* UniqueAttackSequence = nullptr;

	/// Where will this window of opportunity  be opened at
	///		-This can happen anywhere from 0 to ComboSequence - 1.
	///		-Will not play if UniqueAnimationSequence is null, or uninitialized
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int UniqueAttackIndex = 0;

	/// The cooldown (in seconds) after the attack has been initiated
	///		-This is additive of the UniqueAttackSequence's play length (in seconds)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float UniqueAttackCooldown = 0;

	/// Components of this class
	
	/// Mesh of this weapon
	///		-Can be uninitialized, although not recommended
	///		-Collision will be based off of this mesh (Simple Collision)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* WeaponMeshComponent = nullptr;
	/// Collision of this Weapon
	///		-This is a box that attaches to a socket on the mesh itself
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class UBoxComponent* WeaponCollisionBox = nullptr;

	/// Actors UAnimSequence or UAnimBlueprint after they've been reset (Bringing them to their default state)
	UPROPERTY(BlueprintReadOnly)
		class UAnimInstance* DefaultAnimInstance = nullptr;
	
	///Equipped Actors movement component to be set disabled and re-enabled when attacking
	UPROPERTY(BlueprintReadOnly)
		class UCharacterMovementComponent* EquippedCharacterMovementComponent = nullptr;

	///Combo Booleans

	///Boolean for when the attack started (prevent spamming attack inputs)
	bool IsAttacking = false;

	///Boolean Handler for NextAttack(), and to prevent the player attacking again
	bool AttackDelay = false;

	///Boolean Handler for UniqueAttack()
	bool UniqueAttackDelay = false;

	///Boolean for the Unique Attack call
	bool CanUniqueAttack = false;

	///Timer Handles

	///This TimerHandler is to insure that after some time has passed, the function 
	FTimerHandle UniqueAttackDelayTimer;

	//Collision Delegates
	UFUNCTION()
		virtual void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	UFUNCTION()
		virtual void OnComponentHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit); 

};
