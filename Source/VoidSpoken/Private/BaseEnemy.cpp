// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"

ABaseEnemy::ABaseEnemy()
{
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	
}

float ABaseEnemy::GetAttack()
{
	return Attack;
}

void ABaseEnemy::SetAttack(float Atk)
{
	Attack = Atk;
}

float ABaseEnemy::GetWalkSpeed()
{
	return WalkSpeed;
}

void ABaseEnemy::SetWalkSpeed(float Walk)
{
	WalkSpeed = Walk;
}

float ABaseEnemy::GetRunSpeed()
{
	return RunSpeed;
}

void ABaseEnemy::SetRunSpeed(float Run)
{
	RunSpeed = Run;
}

float ABaseEnemy::GetDamageMultiplier()
{
	return DamageMultiplier;
}

void ABaseEnemy::SetDamageMultiplier(float DMult)
{
	DamageMultiplier = DMult;
}

void ABaseEnemy::AttackTrace(UAnimMontage* AnimTrigger)
{
}

void ABaseEnemy::OnSeePawn(APawn* OtherPawn)
{
}

void ABaseEnemy::TriggerAttack()
{
}

void ABaseEnemy::EnterCombat(APawn* OtherPawn, bool Cooldown)
{
}

