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

	//Initialize Pawn Sensing Stats


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

GatekeeperState AGatekeeper::GetState()
{
	return GKState;
}

void AGatekeeper::SetState(GatekeeperState state)
{
	GKState = state;
}

void AGatekeeper::BeginPlay()
{
	Super::BeginPlay();

	BehaviourChange(GatekeeperState::Start);
}

void AGatekeeper::BehaviourChange(GatekeeperState state)
{
	SetState(state);
	BehaviourStateEvent();
}

void AGatekeeper::BehaviourStateEvent()
{
	switch (GKState)
	{
	case GatekeeperState::Start:
		//MoveToActor()
		break;
	case GatekeeperState::Chase:

		break;
	case GatekeeperState::HeavyAttack:

		break;
	case GatekeeperState::SummonPortals:

		break;
	case GatekeeperState::Staggered:

		break;
	case GatekeeperState::Dead:

		break;
	}
}

