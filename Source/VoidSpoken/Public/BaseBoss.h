// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "BaseBoss.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class VOIDSPOKEN_API ABaseBoss : public ABaseEnemy
{
	GENERATED_BODY()
	
public:
	ABaseBoss();

	float GetHPThresholdHigh();
	void SetHPThresholdHigh(float ThreshHigh);

	float GetHPThresholdMed();
	void SetHPThresholdMed(float ThreshMed);

	float GetHPThresholdLow();
	void SetHPThresholdLow(float ThreshLow);

	float GetMinAttackDelay();
	void SetMinAttackDelay(float MinDelay);

	float GetMaxAttackDelay();
	void SetMaxAttackDelay(float MaxDelay);

	virtual void AttackTrace(UAnimMontage* AnimTrigger) override;

private:
	float HPThresholdHigh = 0;
	float HPThresholdMed = 0;
	float HPThresholdLow = 0;
	float MinAttackDelay = 0;
	float MaxAttackDelay = 0;

};
