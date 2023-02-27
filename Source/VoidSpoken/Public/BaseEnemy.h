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
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTriggerDelegate, ABaseEnemy*, EnemyTrigger);

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

	UPROPERTY(BlueprintAssignable, Category = "Death")
		FTriggerDelegate OnDeathTrigger;

	UFUNCTION(BlueprintPure)
		enum EEnemyType GetEnemyType() { return EType; }

	void SetEnemyType(EEnemyType type) { EType = type; }

	UFUNCTION(BlueprintCallable)
		float GetAttack() { return Attack; }
	UFUNCTION(BlueprintCallable)
		void SetAttack(float Atk) { Attack = Atk; }

	UFUNCTION(BlueprintCallable)
		float GetWalkSpeed() { return WalkSpeed; }
	UFUNCTION(BlueprintCallable)
		void SetWalkSpeed(float Walk) { WalkSpeed = Walk; }

	UFUNCTION(BlueprintCallable)
		float GetRunSpeed() { return RunSpeed; }
	UFUNCTION(BlueprintCallable)
		void SetRunSpeed(float Run) { RunSpeed = Run; }

	UFUNCTION(BlueprintCallable)
		float GetDamageMultiplier() { return DamageMultiplier; }
	UFUNCTION(BlueprintCallable)
		void SetDamageMultiplier(float DMult) { DamageMultiplier = DMult; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PawnSensing)
		class UPawnSensingComponent* PawnSensing;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		APawn* AttackTarget = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		AEnemyWeapon* EquippedWeapon;

	virtual void AttackTrace(UAnimMontage* AnimTrigger) {}

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float Attack = 0;

	UFUNCTION()
		virtual void OnSeePawn(APawn* OtherPawn) {}

	UPROPERTY(EditAnywhere)
		TArray<APatrolPoint*> PatrolPoints;

	float TimeOfLastAttack = 0;

	virtual void TriggerAttack() {}
	virtual void EnterCombat(APawn* OtherPawn, bool Cooldown) {}
	virtual bool CheckLineOfSight(AActor* OtherActor) { return false; }
	virtual void SetCombatIdle() {}
	virtual void SetCirclePlayer(bool RandomizeDirection, float AdditionalDistance) {}

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
