// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/CapsuleComponent.h" 
#include "EnemyWeapon.h"
#include "BaseEnemy.generated.h"


/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBroadcastDelegate);

UCLASS(Abstract)
class VOIDSPOKEN_API ABaseEnemy : public ABaseEntity
{
	GENERATED_BODY()

public:
	ABaseEnemy();

	UPROPERTY(BlueprintAssignable, Category = "HealthBar")
		FBroadcastDelegate UpdateHealthBar;

	UFUNCTION(BlueprintCallable)
		float GetAttack();
	UFUNCTION(BlueprintCallable)
		void SetAttack(float Atk);

	UFUNCTION(BlueprintCallable)
		float GetWalkSpeed();
	UFUNCTION(BlueprintCallable)
		void SetWalkSpeed(float Walk);

	UFUNCTION(BlueprintCallable)
		float GetRunSpeed();
	UFUNCTION(BlueprintCallable)
		void SetRunSpeed(float Run);

	UFUNCTION(BlueprintCallable)
		float GetDamageMultiplier();
	UFUNCTION(BlueprintCallable)
		void SetDamageMultiplier(float DMult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PawnSensing)
		class UPawnSensingComponent* PawnSensing;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		APawn* AttackTarget = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		AEnemyWeapon* EquippedWeapon;

	virtual void AttackTrace(UAnimMontage* AnimTrigger);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float Attack = 0;

private:

	float WalkSpeed = 0;
	float RunSpeed = 0;
	float DamageMultiplier = 10;
};
