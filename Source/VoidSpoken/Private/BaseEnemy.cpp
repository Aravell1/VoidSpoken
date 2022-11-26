// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"

ABaseEnemy::ABaseEnemy()
{
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	
}

float ABaseEnemy::GetMaxHealth()
{
	return MaxHealth;
}

void ABaseEnemy::SetMaxHealth(float MHP)
{
	MaxHealth = MHP;
}

float ABaseEnemy::GetAttack()
{
	return Attack;
}

void ABaseEnemy::SetAttack(float Atk)
{
	Attack = Atk;
}

float ABaseEnemy::GetDefense()
{
	return Defense;
}

void ABaseEnemy::SetDefense(float Def)
{
	Defense = Def;
}

float ABaseEnemy::GetHealth()
{
	return Health;
}

void ABaseEnemy::SetHealth(float HP)
{
	Health = HP;
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

