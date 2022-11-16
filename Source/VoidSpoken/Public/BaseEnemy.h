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

	float GetMaxHealth();
	void SetMaxHealth(float MHP);

	float GetAttack();
	void SetAttack(float Atk);

	float GetDefense();
	void SetDefense(float Def);

	float GetHealth();
	void SetHealth(float HP);

	float GetWalkSpeed();
	void SetWalkSpeed(float Walk);

	float GetRunSpeed();
	void SetRunSpeed(float Run);

	float GetDamageMultiplier();
	void SetDamageMultiplier(float DMult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PawnSensing)
		class UPawnSensingComponent* PawnSensing;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = CapsuleComponent)
	UCapsuleComponent* CapsuleComponent;


	
private:
	float Health = 0;
	float Attack = 0;
	float Defense = 0;
	float MaxHealth = 0;
	float WalkSpeed = 0;
	float RunSpeed = 0;
	float DamageMultiplier = 10;
	APawn* AttackTarget = nullptr;
};
