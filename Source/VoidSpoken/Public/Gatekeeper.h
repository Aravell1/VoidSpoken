// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBoss.h"
#include "AIController.h"
#include "Math/TransformNonVectorized.h"
#include "Gatekeeper.generated.h"

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
/**
 * 
 */
UCLASS()
class VOIDSPOKEN_API AGatekeeper : public ABaseBoss
{
	GENERATED_BODY()

public:
	AGatekeeper();

	float GetAttackMultiplier();
	void SetAttackMultiplier(float AttackMult);

	float GetDefenseMultiplier();
	void SetDefenseMultiplier(float DefMult);

	enum GatekeeperState GetState();
	void SetState(GatekeeperState state);

	void BeginPlay();

	void BehaviourChange(GatekeeperState state);

	//UFUNCTION(BlueprintNativeEvent, Category = "Behaviour Change")
	void BehaviourStateEvent();

	UPROPERTY(EditAnywhere)
		FTransform BossStartPoint;

private:
	float AttackMultiplier = 0;
	float DefenseMultiplier = 0;
	
	GatekeeperState GKState = Start;

	
};
