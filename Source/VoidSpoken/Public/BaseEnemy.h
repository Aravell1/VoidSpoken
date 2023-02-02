// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/CapsuleComponent.h" 
#include "EnemyWeapon.h"
#include "PatrolPoint.h"
#include "BaseEnemy.generated.h"


/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBroadcastDelegate);

UENUM()
enum EEnemyType
{
	Melee	UMETA(DisplayName = "Melee"),
	Ranged	UMETA(DisplayName = "Ranged")
};

UCLASS(Abstract)
class VOIDSPOKEN_API ABaseEnemy : public ABaseEntity
{
	GENERATED_BODY()

public:
	ABaseEnemy();

	UPROPERTY(BlueprintAssignable, Category = "HealthBar")
		FBroadcastDelegate UpdateHealthBar;

	UFUNCTION(BlueprintPure)
		enum EEnemyType GetEnemyType();

	void SetEnemyType(EEnemyType type);

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

	UFUNCTION()
		virtual void OnSeePawn(APawn* OtherPawn);

	UPROPERTY(EditAnywhere)
		TArray<APatrolPoint*> PatrolPoints;

	float TimeOfLastAttack = 0;

	virtual void TriggerAttack();
	virtual void EnterCombat(APawn* OtherPawn, bool Cooldown);
	virtual bool CheckLineOfSight(AActor* OtherActor);
	virtual void SetCombatIdle();
	virtual void SetCirclePlayer();

	UPROPERTY(VisibleAnywhere)
		bool bInCombat = false;
	UPROPERTY(VisibleAnywhere)
		bool bCanAttack = false;

private:

	UPROPERTY(EditDefaultsOnly)
		TEnumAsByte<EEnemyType> EType = EEnemyType::Melee;

	float WalkSpeed = 0;
	float RunSpeed = 0;
	float DamageMultiplier = 10;
};
