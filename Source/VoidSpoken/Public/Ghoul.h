// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "BaseEnemy.h"
#include "Animation/AnimMontage.h" 
#include "Animation/AnimInstance.h"
#include "GhoulAIController.h"
#include "SpikeProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/Actor.h" 
#include "Components/BoxComponent.h" 
#include "Kismet/KismetSystemLibrary.h" 
#include "NavigationSystem.h"
#include "PatrolPoint.h"
#include "Ghoul.generated.h"

/**
 * 
 */
UENUM()
enum EGhoulState
{
	Idle	UMETA(DisplayName = "Idle"),
	Patrol	UMETA(DisplayName = "Patrol"),
	Attack	UMETA(DisplayName = "Attack"),
	AttackCooldown	UMETA(DisplayName = "AttackCooldown"),
	Dead	UMETA(DisplayName = "Dead")
};

UENUM()
enum EGhoulType
{
	Melee	UMETA(DisplayName = "Melee"),
	Ranged	UMETA(DisplayName = "Ranged")
};

UCLASS()
class VOIDSPOKEN_API AGhoul : public ABaseEnemy
{
	GENERATED_BODY()
	
public:
	AGhoul();

	UFUNCTION(BlueprintPure)
		enum EGhoulState GetState();

	void SetState(EGhoulState state);

	UFUNCTION(BlueprintPure)
		enum EGhoulType GetType();

	void SetGhoulType(EGhoulType type);

	void BehaviourStateEvent();

	void SetAttacking(UAnimMontage* Montage, bool Attacking);
	void SetAttackingRight(bool right);
	void SetAttackingLeft(bool left);

	void TriggerSpikes(UAnimMontage* Montage);
	void SpikeBurst();
	void SpikeThrow();
	void CreateSpike(FRotator Rotation, FVector Location, bool UseSpikeCollision);

	void OnSeePawn(APawn* OtherPawn) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UAnimMontage* Attack1Montage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UAnimMontage* Attack2Montage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UAnimMontage* Attack3Montage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UAnimMontage* RangedAttackMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UAnimMontage* BurstMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UAnimMontage* RandomMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UBoxComponent* HitBoxRight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UBoxComponent* HitBoxLeft;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UBoxComponent* ThrowPoint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UBoxComponent* HeadLocation;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ASpikeProjectile> Spike;

protected:

	void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) override;
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
	UFUNCTION()
		void OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
		void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	UPROPERTY(EditAnywhere)
		TArray<APatrolPoint*> PatrolPoints;

	int PatrolIndex = 0;

	void SetSpeed();
	void AttackDelay();
	void IdleDelay();
	void StopMovement();
	void Death();
	void AttackCooldown();

	void CheckPatrolReset();
	void PatrolReset();
	bool GetHasLineOfSight();
	bool TestPathExists(AActor* Target);

	UPROPERTY(VisibleAnywhere)
		TEnumAsByte<EGhoulState> GhState = EGhoulState::Idle;

	UPROPERTY(EditDefaultsOnly)
		TEnumAsByte<EGhoulType> GhType = EGhoulType::Melee;

	UPROPERTY()
		TArray<UAnimMontage*> MontageArray;

	AGhoulAIController* AIController;
	FOnMontageEnded MontageEndDelegate;
	FTimerHandle TimerHandle;

	float ReachTargetDistance = 0;
	const float MeleeTargetDistance = 100.0f;
	const float RangedTargetDistance = 2000.0f;

	FTimerHandle PatrolTimerHandle;
	const float PTHandleInterval = 0.5f;
	const float PatrolResetDistance = 4000.0f;
	const float PatrolResetTime = 15.0f;
	float PResetTimer = 0;

	const float BackUpSpeed = 100.0f;
	const float BackOffRange = 500.0f;
	const float BackUpDistance = 300.0f;

	const float IdleTime = 4.0f;
	const float AttackCD = 2.5f;

	const float BurstSpikeSpawnDistance = 50.0f;
	const float BurstRadius = 500.0f;

	bool AttackingRight = false;
	bool AttackingLeft = false;
	bool AttackReset = true;
	bool LockState = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "True"))
	bool bIsDead = false;
};
