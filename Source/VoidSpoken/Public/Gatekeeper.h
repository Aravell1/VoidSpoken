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
#include "../VoidSpokenGameModeBase.h"
#include "GameFramework/Actor.h" 
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

	void OnSeePawn(APawn* OtherPawn) override;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<APortalSpawn> Portal;

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

	UPROPERTY(EditDefaultsOnly)
		float StompImpulseForce = 5000;

	void AttackTrace(UAnimMontage* AnimTrigger) override;
protected:
	void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	void Tick(float DeltaTime) override;
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) override;
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result);
	UFUNCTION()
		void OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted);

private:

	void SetSpeed();
	void AttackDelay();
	void PortalDelay();
	void Enrage();
	void StopMovement();
	void TrackPlayer();
	void SpawnPortals(int PortalCount);
	void Death();
	int HealthCheck(float Damage);
	bool HPThresholdCheck(float HPThreshold, float Damage);
	void UpdateHealth(bool StopMovement, float Damage);
	void RandomizeTimeToRun();

	UPROPERTY(VisibleAnywhere)
		GatekeeperState GKState = GatekeeperState::Start;

	UPROPERTY(VisibleAnywhere)
		TArray<UAnimMontage*> MontageArray;

	AGatekeeperAIController* AIController;
	FOnMontageEnded MontageEndDelegate;
	FTimerHandle TimerHandle;

	float ReachTargetDistance = 320.0f;
	float TimeToRun = 4.0f;
	float RandomTimeToRun = 3.0f;
	float RunTimer = 0;

	float AttackMultiplier = 1;
	float DefenseMultiplier = 1;
	float StompRadius = 750;
	bool HeavyReset = true;
	bool AttackReset = true;
	bool PortalReset = true;
	bool Attacking = false;
	bool Enraged = false;
	bool LockState = false;

	
};
