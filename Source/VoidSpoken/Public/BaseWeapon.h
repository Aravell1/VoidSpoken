// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "BaseWeapon.generated.h"

UCLASS(Abstract)
class VOIDSPOKEN_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();

	//Weapon Parameters
	
	/// Base Damage of this Weapon
	///		-Any scaling involving the damage of the Attack, this value will influence
	///		-Can be set to any float value
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = ( NoGetter ))
		float BaseDamage = 0;

	/// The Actor that has equipped this weapon
	///		-For purposes of unwanted self damage infliction
	///		-This will also be used to equip to this Actor
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		ACharacter* EquippedActor;

	//Getter Statements

	UFUNCTION(BlueprintPure, BlueprintCallable)
		float GetBaseDamage();
	UFUNCTION(BlueprintPure, BlueprintCallable)
		int GetCurrentComboIndex();
	UFUNCTION(BlueprintPure, BlueprintCallable)
		int GetComboLength();

	//Weapon Functions

	UFUNCTION(BlueprintCallable)
		void Equip(ACharacter* EquippingActor);
	UFUNCTION(BlueprintCallable)
		void Attack();
	UFUNCTION(BlueprintCallable)
		void NextAttack();
	UFUNCTION(BlueprintCallable)
		void UniqueAttack();
	UFUNCTION(BlueprintCallable)
		void Reset();

	//Timer Handles
	FTimerHandle AttackDelay;
	FTimerHandle ResetDelay;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// This Array contains all the Animation Montages for the Attack of this weapon.
	///		-This cannot be unintialized
	///		-This will loop constantly when this runs through the whole Sequence
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		TArray<UAnimMontage*> ComboMontage = {};

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
	///		-Will not play if UniqueAnimationSequence is null, or unintialized
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int UniqueAttackIndex = 0;

	/// Mesh of this weapon
	///		-Can be uninitialized, although not recommended
	///		-Collision will be based off of this mesh (Simple Collision)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* WeaponMeshComponent;
		

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
