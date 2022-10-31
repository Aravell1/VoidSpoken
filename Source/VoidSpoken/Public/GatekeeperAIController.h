// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Gatekeeper.h"
#include "Math/UnrealMathUtility.h"
#include "GatekeeperAIController.generated.h"

class UBehaviorTreeComponent;

/**
 * 
 */
UENUM()
enum GatekeeperState
{
	Start	UMETA(DisplayName = "Start"),
	Chase	UMETA(DisplayName = "Chase"),
	HeavyAttack	UMETA(DisplayName = "HeavyAttack"),
	SummonPortals	UMETA(DisplayName = "SummonPortals"),
	Staggered	UMETA(DisplayName = "Staggered"),
	Dead	UMETA(DisplayName = "Dead")
};

UCLASS()
class VOIDSPOKEN_API AGatekeeperAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AGatekeeperAIController();
	virtual void SeePlayer();

	enum GatekeeperState GetState();
	void SetState(GatekeeperState state);

	void BehaviourStateEvent();
	void BehaviourChange(GatekeeperState state);



protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	void OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted);
	void SetSpeed();
	void AttackDelay();
	void Enrage();
	void StopMovement();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
		TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
		TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
		TObjectPtr<UBlackboardComponent> BlackboardComponent;

	GatekeeperState GKState = Start;
	AGatekeeper* Gatekeeper;
	APawn* PlayerPawn = nullptr;
	FOnMontageEnded EndDelegate;
	TArray<UAnimMontage*> MontageArray = {Gatekeeper->HeavyAttackMontage, Gatekeeper->StompMontage, Gatekeeper->BeamMontage};

	float ReachTargetDistance = 320.0f;
	bool HeavyReset = true;
	bool AttackReset = true;
	bool PortalReset = true;
	bool Attacking = false;
	bool Enraged = false;




};
