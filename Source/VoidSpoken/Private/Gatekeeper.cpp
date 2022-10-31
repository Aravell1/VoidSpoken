// Fill out your copyright notice in the Description page of Project Settings.


#include "Gatekeeper.h"

AGatekeeper::AGatekeeper()
{
	//Initialize General Stats
	SetAttack(30);
	SetDefense(20);
	SetWalkSpeed(350);
	SetRunSpeed(500);
	SetMaxHealth(200);
	SetHealth(GetMaxHealth());

	//Initialize Boss Stats
	SetMinAttackDelay(1.5f);
	SetMaxAttackDelay(2);
	SetHPThresholdHigh(0.75f);
	SetHPThresholdMed(0.5f);
	SetHPThresholdLow(0.25f);

	//Initialize Gatekeeper Stats
	SetAttackMultiplier(1.25f);
	SetDefenseMultiplier(0.75f);

	BossStartPoint = GetActorLocation() + GetActorForwardVector() * 300;

	//Initialize Character Movement Stats
	GetCharacterMovement()->MaxWalkSpeed = GetWalkSpeed();

	
}

float AGatekeeper::GetAttackMultiplier()
{
	return AttackMultiplier;
}

void AGatekeeper::SetAttackMultiplier(float AttackMult)
{
	AttackMultiplier = AttackMult;
}

float AGatekeeper::GetDefenseMultiplier()
{
	return DefenseMultiplier;
}

void AGatekeeper::SetDefenseMultiplier(float DefMult)
{
	DefenseMultiplier = DefMult;
}



void AGatekeeper::BeginPlay()
{
	Super::BeginPlay();

	
}







