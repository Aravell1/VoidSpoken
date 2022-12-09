// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "StatsMasterClass.h"
#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "EnemyWeapon.generated.h"

/**
 * 
 */
UCLASS()
class VOIDSPOKEN_API AEnemyWeapon : public AActor
{
	GENERATED_BODY()

public:
	AEnemyWeapon();

	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	///Getter Statements

	/// Function: GetBaseDamage()
	///		-Returns the float BaseDamage
	UFUNCTION(BlueprintPure, BlueprintCallable)
		float GetBaseDamage();

	UFUNCTION(BlueprintCallable)
		void SetBaseDamage(float Damage);

	/// Function: GetEquippedCharacter() 
	///		-Returns a ACharacter* of the variable EquippedCharacter
	UFUNCTION(BlueprintPure, BlueprintCallable)
		ACharacter* GetEquippedCharacter();

	/// Function: Equip(ACharacter* EquippingCharacter) 
///		-This functions sets the variable EquippedCharacter
///		-Will Also set this Weapon's owner as EquippedCharacter
///		-Initialises the UAnimInstance from the Default (At the time of this Equip(), what was the UAnimInstance the Character had)
///		-Initialises the UCharacterMovementComponent from the given EquippedCharacter 
	UFUNCTION(BlueprintCallable)
		virtual void Equip(ACharacter* EquippingCharacter, float Damage);

	/// Function: DealDamage()
	///		-Checks if the Attached Box Collider is overlapping any Actors (Preferably if their from a BaseEntity class)
	///		-Length of this function is determined in each of the ComboAnimationMontage
	///		-Uses the Notify and NotifyState systems in each Animation
	UFUNCTION(BlueprintCallable)
		virtual void DealDamage(class AActor* OtherActor);

	void SetIsAttacking(bool IsAttacking);

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

	//Combo Booleans

	///Boolean for when the attack started (prevent spamming attack inputs)
	bool Attacking = false;

	//Collision Delegates
	UFUNCTION()
		virtual void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
