// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBoss.h"
#include "Math/TransformNonVectorized.h"
#include "Animation/AnimMontage.h" 
#include "Animation/AnimInstance.h"
#include "GatekeeperAIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "PortalSpawn.h"
#include "Components/SphereComponent.h" 
#include "GatekeeperTransforms.h"
#include "Kismet/KismetSystemLibrary.h" 
#include "Gatekeeper.generated.h"


/**
 * 
 */
UENUM()
enum class GatekeeperState
{
	Start	UMETA(DisplayName = "Start"),
	Chase	UMETA(DisplayName = "Chase"),
	HeavyAttack	UMETA(DisplayName = "HeavyAttack"),
	SummonPortals	UMETA(DisplayName = "SummonPortals"),
	Staggered	UMETA(DisplayName = "Staggered"),
	Dead	UMETA(DisplayName = "Dead")
};

UCLASS()
class VOIDSPOKEN_API AGatekeeper : public ABaseBoss
{
	GENERATED_BODY()

public:
	AGatekeeper();

	enum GatekeeperState GetState();
	void SetState(GatekeeperState state);

	void BehaviourStateEvent();
	void BehaviourChange(GatekeeperState state);

	float GetAttackMultiplier();
	void SetAttackMultiplier(float AttackMult);

	float GetDefenseMultiplier();
	void SetDefenseMultiplier(float DefMult);

	UFUNCTION()
		void OnSeePawn(APawn* OtherPawn);

	void BeginPlay();

	/*UPROPERTY(VisibleAnywhere, Category = SkeletalMesh)
		class USkeletalMesh* MeshContainer;

	UPROPERTY(VisibleAnywhere, Category = SkeletalMesh)
		class USkeletalMeshComponent* PlayerMesh;*/

	/*UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* Weapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USphereComponent* WeaponCollider;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		USphereComponent* ChestLocation;*/

	UPROPERTY(EditAnywhere)
		TArray<AGatekeeperTransforms*> PortalSpawns = { nullptr, nullptr, nullptr };
	UPROPERTY(EditAnywhere)
		AGatekeeperTransforms* BossStartPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UAnimMontage* BaseAttackMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UAnimMontage* HeavyAttackMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UAnimMontage* StompMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UAnimMontage* BeamMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UAnimMontage* SummonPortalMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UAnimMontage* EnrageMontage = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		UAnimMontage* RandomMontage = nullptr;

private:

	UFUNCTION()
		void OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
		void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);

	void SetSpeed();
	void AttackDelay();
	void Enrage();
	void StopMovement();
	void TrackPlayer();
	void SpawnPortals(int PortalCount);
	void Death();
	int HealthCheck(float Damage);
	bool HPThresholdCheck(float HPThreshold, float Damage);
	void UpdateHealth(bool StopMovement, float Damage);
	void AttackTrace(UAnimMontage* AnimTrigger);

	UPROPERTY(VisibleAnywhere)
		GatekeeperState GKState = GatekeeperState::Start;

	AGatekeeperAIController* AIController;
	TArray<UAnimMontage*> MontageArray = { HeavyAttackMontage, StompMontage, BeamMontage };
	FOnMontageEnded MontageEndDelegate;

	float ReachTargetDistance = 320.0f;
	float AttackMultiplier = 1;
	float DefenseMultiplier = 1;
	/*float WeaponRadius = 60;
	float ChestRadius = 30;*/
	float StompRadius = 500;
	bool HeavyReset = true;
	bool AttackReset = true;
	bool PortalReset = true;
	bool Attacking = false;
	bool Enraged = false;

	
};
