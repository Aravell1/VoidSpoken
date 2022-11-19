// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/CapsuleComponent.h" 
#include "BaseEnemy.generated.h"

/**
 * 
 */
UCLASS()
class VOIDSPOKEN_API ABaseEnemy : public ABaseEntity
{
	GENERATED_BODY()

public:
	ABaseEnemy();

	UFUNCTION(BlueprintCallable)
		float GetMaxHealth();
	UFUNCTION(BlueprintCallable)
		void SetMaxHealth(float MHP);

	UFUNCTION(BlueprintCallable)
		float GetAttack();
	UFUNCTION(BlueprintCallable)
		void SetAttack(float Atk);

	UFUNCTION(BlueprintCallable)
		float GetDefense();
	UFUNCTION(BlueprintCallable)
		void SetDefense(float Def);

	UFUNCTION(BlueprintCallable)
		float GetHealth();
	UFUNCTION(BlueprintCallable)
		void SetHealth(float HP);

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
	
private:
	UPROPERTY(VisibleAnywhere)
		float Health = 0;
	float Attack = 0;
	float Defense = 0;
	float MaxHealth = 0;
	float WalkSpeed = 0;
	float RunSpeed = 0;
	float DamageMultiplier = 10;
};
