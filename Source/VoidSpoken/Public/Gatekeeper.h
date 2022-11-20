// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseBoss.h"
#include "Math/TransformNonVectorized.h"
#include "Animation/AnimMontage.h" 
#include "Animation/AnimInstance.h"
#include "Gatekeeper.generated.h"


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

	void BeginPlay();


	UPROPERTY(EditAnywhere)
		FVector3d BossStartPoint;

	UPROPERTY(EditAnywhere)
		UAnimMontage* BaseAttackMontage = nullptr;
	UPROPERTY(EditAnywhere)
		UAnimMontage* HeavyAttackMontage = nullptr;
	UPROPERTY(EditAnywhere)
		UAnimMontage* StompMontage = nullptr;
	UPROPERTY(EditAnywhere)
		UAnimMontage* BeamMontage = nullptr;
	UPROPERTY(EditAnywhere)
		UAnimMontage* SummonPortalMontage = nullptr;
	UPROPERTY(EditAnywhere)
		UAnimMontage* EnrageMontage = nullptr;
	UPROPERTY(EditAnywhere)
		UAnimMontage* RandomMontage = nullptr;

private:
	float AttackMultiplier = 0;
	float DefenseMultiplier = 0;
	
};
