// Fill out your copyright notice in the Description page of Project Settings.


#include "GatekeeperAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

AGatekeeperAIController::AGatekeeperAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));

	Gatekeeper = Cast<AGatekeeper>(GetOwner());

	if (Gatekeeper)
	{
		MontageArray = { Gatekeeper->HeavyAttackMontage, Gatekeeper->StompMontage, Gatekeeper->BeamMontage };

		FOnMontageEnded MontageEndDelegate;
		MontageEndDelegate.BindUObject(this, &AGatekeeperAIController::OnAnimationEnded);
		Gatekeeper->GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, Gatekeeper->BaseAttackMontage);
		Gatekeeper->GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, Gatekeeper->HeavyAttackMontage);
		Gatekeeper->GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, Gatekeeper->StompMontage);
		Gatekeeper->GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, Gatekeeper->BeamMontage);
		Gatekeeper->GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, Gatekeeper->SummonPortalMontage);
		Gatekeeper->GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, Gatekeeper->EnrageMontage);

	}
}

void AGatekeeperAIController::SeePlayer()
{
	PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	SetFocus(PlayerPawn);

	BehaviourChange(GatekeeperState::Chase);

}

void AGatekeeperAIController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(BehaviorTree.Get()))
	{
		RunBehaviorTree(BehaviorTree.Get());
		BehaviorTreeComponent->StartTree(*BehaviorTree.Get());
	}

	BehaviourChange(GatekeeperState::Start);
}

void AGatekeeperAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsValid(Blackboard.Get()) && IsValid(BehaviorTree.Get()))
	{
		Blackboard->InitializeBlackboard(*BehaviorTree.Get()->BlackboardAsset.Get());
	}
}

void AGatekeeperAIController::StopMovement()
{
	Gatekeeper->GetCharacterMovement()->StopMovementImmediately();
	Gatekeeper->GetCharacterMovement()->MaxWalkSpeed = 0;
	Attacking = true;
}

GatekeeperState AGatekeeperAIController::GetState()
{
	return GKState;
}

void AGatekeeperAIController::SetState(GatekeeperState state)
{
	GKState = state;
}

void AGatekeeperAIController::BehaviourChange(GatekeeperState state)
{
	SetState(state);
	BehaviourStateEvent();
}

void AGatekeeperAIController::BehaviourStateEvent()
{
	switch (GKState)
	{
	case GatekeeperState::Start:
		MoveToLocation(Gatekeeper->BossStartPoint);
		break;
	case GatekeeperState::Chase:
		if (MoveToActor(PlayerPawn, ReachTargetDistance))
		{
			if (AttackReset)
			{
				AttackReset = false;
				Gatekeeper->GetMesh()->GetAnimInstance()->Montage_Play(Gatekeeper->BaseAttackMontage);
				StopMovement();
			}
		}
		break;
	case GatekeeperState::HeavyAttack:
		if (MoveToActor(PlayerPawn, ReachTargetDistance))
		{
			if (HeavyReset)
			{
				HeavyReset = false;
				int index = FMath::RandRange(0, MontageArray.Num() - 1);
				Gatekeeper->GetMesh()->GetAnimInstance()->Montage_Play(MontageArray[index]);
				StopMovement();
			}
		}
		break;
	case GatekeeperState::SummonPortals:
		Attacking = false;
		if (PortalReset)
		{
			PortalReset = false;
			Gatekeeper->GetMesh()->GetAnimInstance()->Montage_Play(Gatekeeper->SummonPortalMontage);
			
			StopMovement();
		}
		break;
	case GatekeeperState::Staggered:

		break;
	case GatekeeperState::Dead:

		break;
	}

}

void AGatekeeperAIController::OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (Montage == Gatekeeper->BaseAttackMontage)
		{
			Attacking = false;
			SetSpeed();
			AttackReset = true;
			BehaviourChange(HeavyAttack);
		}
		else if ((Montage == Gatekeeper->HeavyAttackMontage || Montage == Gatekeeper->StompMontage || Montage == Gatekeeper->BeamMontage) && GKState == GatekeeperState::HeavyAttack)
		{
			Attacking = false;
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle, 
				this, 
				&AGatekeeperAIController::AttackDelay, 
				FMath::RandRange(Gatekeeper->GetMinAttackDelay(), Gatekeeper->GetMaxAttackDelay()));
		}
		else if (Montage == Gatekeeper->SummonPortalMontage)
		{
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle,
				this,
				&AGatekeeperAIController::AttackDelay,
				1.0f);
			if (Gatekeeper->GetHealth() <= Gatekeeper->GetMaxHealth() * Gatekeeper->GetHPThresholdLow())
			{
				AttackReset = true;
				HeavyReset = true;
				Enrage();
			}
			else 
			{
				Gatekeeper->GetMesh()->GetAnimInstance()->Montage_Play(Gatekeeper->BeamMontage);
			}
		}
		else if (Montage == Gatekeeper->BeamMontage && GKState == GatekeeperState::SummonPortals)
		{
			SetSpeed();
			AttackReset = true;
			HeavyReset = true;
			PortalReset = true;
			BehaviourChange(Chase);
		}
		else if (Montage == Gatekeeper->EnrageMontage)
		{
			SetSpeed();
			BehaviourChange(HeavyAttack);
		}
	}
}

void AGatekeeperAIController::SetSpeed()
{
	if (Enraged)
	{
		Gatekeeper->GetCharacterMovement()->MaxWalkSpeed = Gatekeeper->GetRunSpeed();
	}
	else
	{
		Gatekeeper->GetCharacterMovement()->MaxWalkSpeed = Gatekeeper->GetWalkSpeed();
	}
}

void AGatekeeperAIController::AttackDelay()
{
	HeavyReset = true;
	SetSpeed();
	BehaviourChange(Chase);
}

void AGatekeeperAIController::Enrage()
{
	Enraged = true;
	Gatekeeper->SetAttack(Gatekeeper->GetAttack() * Gatekeeper->GetAttackMultiplier());
	Gatekeeper->GetMesh()->GetAnimInstance()->Montage_Play(Gatekeeper->EnrageMontage);
}

