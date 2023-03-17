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
#include "Ghoul.generated.h"

/**
 * 
 */
UENUM()
enum EGhoulState
{
	Idle	UMETA(DisplayName = "Idle"),
	Patrol	UMETA(DisplayName = "Patrol"),
	//CallAllies	UMETA(DisplayName = "Call Allies"),
	Chase	UMETA(DisplayName = "Chase"),
	Attack	UMETA(DisplayName = "Attack"),
	AttackCooldown	UMETA(DisplayName = "Attack Cooldown"),
	CombatIdle	UMETA(DisplayName = "Combat Idle"),
	CirclePlayer	UMETA(DisplayName = "Circle Player"),
	Staggered UMETA(DisplayName = "Staggered"),
	Dead	UMETA(DisplayName = "Dead")
};

UCLASS()
class VOIDSPOKEN_API AGhoul : public ABaseEnemy
{
	GENERATED_BODY()
	
public:
	AGhoul();

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		UEnvQuery* FindLocationWithLOSEQS;

	UFUNCTION(BlueprintPure)
		enum EGhoulState GetState();

	void SetState(EGhoulState state);

	void BehaviourStateEvent();

	void SetAttacking(UAnimMontage* Montage, bool Attacking);

	UFUNCTION(BlueprintImplementableEvent)
		void TriggerHitEvent();
	void TriggerAttack() override;
	void BeginAttack();

	void TriggerSpikes(UAnimMontage* Montage);
	void SpikeBurst();
	void SpikeThrow();
	void CreateSpike(FRotator Rotation, FVector Location, bool UseSpikeCollision, float InitVel = 0);

	bool TestPathExists(AActor* Target) override;
	bool TestPathExists(FVector Target) override;
	void OnSeePawn(APawn* OtherPawn) override;
	void OnStaggered() override;
	bool CheckLineOfSight(AActor* OtherActor) override;
	void SetCombatIdle() override;
	void SetCirclePlayer(bool RandomizeDirection, float AdditionalDistance) override;

	UFUNCTION(BlueprintImplementableEvent)
		void PlaySoundAtLocation(int SoundIndex);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Montages")
		UAnimMontage* IdleBreak01Montage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Montages")
		UAnimMontage* IdleBreak02Montage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Montages")
		UAnimMontage* JumpBackMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Montages")
		UAnimMontage* Attack1Montage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Montages")
		UAnimMontage* Attack2Montage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Montages")
		UAnimMontage* Attack3Montage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Montages")
		UAnimMontage* RangedAttackMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Montages")
		UAnimMontage* BurstMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Montages")
		UAnimMontage* StaggerMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Montages")
		UAnimMontage* ScreechMontage = nullptr;
	UPROPERTY(BlueprintReadWrite)
		UAnimMontage* RandomMontage = nullptr;

	UPROPERTY(BlueprintReadWrite)
		UBoxComponent* HitBoxRight;
	UPROPERTY(BlueprintReadWrite)
		UBoxComponent* HitBoxLeft;
	UPROPERTY(BlueprintReadWrite)
		UBoxComponent* ThrowPoint;
	UPROPERTY(BlueprintReadWrite)
		UBoxComponent* HeadLocation;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
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

	int PatrolIndex = 0;

	void SetSpeed();
	void IdleDelay();
	void StopMovement();
	void Death();
	void AttackCooldown();
	void EnterCombat(APawn* OtherPawn, bool Cooldown) override;
	void CirclePlayer();
	void CombatIdle();
	void PlayRandomIdle();
	void AttackLOSCheck();

	void CheckPatrolReset();
	void PatrolReset();

	void AddCurveMovement();

	UPROPERTY(VisibleAnywhere)
		TEnumAsByte<EGhoulState> GhState = EGhoulState::Idle;

	UPROPERTY()
		TArray<UAnimMontage*> MontageArray;

	AGhoulAIController* AIController;
	FOnMontageEnded MontageEndDelegate;
	FTimerHandle TimerHandle;
	FTimerHandle AttackCooldownTimer;

	const float GhoulWalkSpeed = 200.0f;
	const float GhoulRunSpeed = 300.0f;

	float ReachTargetDistance = 0;
	float CheckingDistance = 0;
	float CircleTargetDistance = 0;
	const float MeleeTargetDistance = 125.0f;
	const float RangedTargetDistance = 1250.0f;

	FTimerHandle PatrolTimerHandle;
	const float PTHandleInterval = 0.5f;
	const float PatrolResetDistance = 4000.0f;
	const float PatrolResetTime = 15.0f;
	float PResetTimer = 0;

	const float LOSCheckDuration = 0.5f;
	FTimerHandle LOSCheckTimer;

	const float BackUpSpeed = 100.0f;
	const float BackOffRange = 500.0f;
	const float BackUpDistance = 500.0f;

	const float IdleTime = 4.0f;
	const float AttackCD = 2.5f;

	const float BurstSpikeSpawnDistance = 50.0f;
	const float BurstRadius = 500.0f;
	const float ProjectileSpeed = 1250.0f;

	const float CallAlliesRange = 1200.0f;
	const float MeleeSpreadRange = 500.0f;

	bool AttackingRight = false;
	bool AttackingLeft = false;
	bool AttackReset = true;
	bool LockState = false;
	bool MoveRight = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "True"))
	bool bIsDead = false;
};
