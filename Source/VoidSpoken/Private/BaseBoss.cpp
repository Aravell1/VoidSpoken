// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseBoss.h"

float ABaseBoss::GetHPThresholdHigh()
{
    return HPThresholdHigh;
}

void ABaseBoss::SetHPThresholdHigh(float ThreshHigh)
{
    HPThresholdHigh = ThreshHigh;
}

float ABaseBoss::GetHPThresholdMed()
{
    return HPThresholdMed;
}

void ABaseBoss::SetHPThresholdMed(float ThreshMed)
{
    HPThresholdMed = ThreshMed;
}

float ABaseBoss::GetHPThresholdLow()
{
    return HPThresholdLow;
}

void ABaseBoss::SetHPThresholdLow(float ThreshLow)
{
    HPThresholdLow = ThreshLow;
}

float ABaseBoss::GetMinAttackDelay()
{
    return MinAttackDelay;
}

void ABaseBoss::SetMinAttackDelay(float MinDelay)
{
    MinAttackDelay = MinDelay;
}

float ABaseBoss::GetMaxAttackDelay()
{
    return MaxAttackDelay;
}

void ABaseBoss::SetMaxAttackDelay(float MaxDelay)
{
    MaxAttackDelay = MaxDelay;
}
