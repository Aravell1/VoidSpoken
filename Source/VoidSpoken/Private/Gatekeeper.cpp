// Fill out your copyright notice in the Description page of Project Settings.


#include "Gatekeeper.h"

AGatekeeper::AGatekeeper()
{
	RootComponent = GetCapsuleComponent();
	GetCapsuleComponent()->SetRelativeScale3D(FVector(2.5, 2.5, 2.5));
	GetCapsuleComponent()->SetCapsuleHalfHeight(88);
	GetCapsuleComponent()->SetCapsuleRadius(34);

	if (!GetMesh()->SkeletalMesh)
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshContainer(TEXT("/Game/Blueprints/Bosses/Gatekeeper/Components/Standing_Idle.Standing_Idle"));
		if (MeshContainer.Succeeded())
		{
			GetMesh()->SetSkeletalMesh(MeshContainer.Object);

			GetMesh()->SetupAttachment(GetCapsuleComponent());

			GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
			GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
		}
	}

	//Initialize General Stats
	SetAttack(30);
	SetDefense(20);
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

	//Initialize Character Movement Stats
	SetWalkSpeed(350);
	SetRunSpeed(500);
	GetCharacterMovement()->MaxWalkSpeed = 100;


	if (PawnSensing)
	{
		PawnSensing->SightRadius = 3500.0f;
		PawnSensing->SetPeripheralVisionAngle(85.0f);
		PawnSensing->bOnlySensePlayers = true;
		PawnSensing->OnSeePawn.AddDynamic(this, &AGatekeeper::OnSeePawn);
	}
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

void AGatekeeper::OnSeePawn(APawn* OtherPawn)
{

	AttackTarget = OtherPawn;
	AIController->SeePlayer(AttackTarget);

	SetSpeed();

	//Set HP Widget Component Visible

	SetState(GatekeeperState::Chase);

	PawnSensing->SetSensingUpdatesEnabled(false);
}

void AGatekeeper::BeginPlay()
{
	Super::BeginPlay();

	if (!AIController)
		AIController = GetController<AGatekeeperAIController>();
	if (AIController)
		AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AGatekeeper::OnMoveCompleted);
	
	MontageEndDelegate.BindUObject(this, &AGatekeeper::OnAnimationEnded);
	MontageArray.Add(HeavyAttackMontage);
	MontageArray.Add(StompMontage);
	MontageArray.Add(BeamMontage);

	SetState(GatekeeperState::Start);
}

void AGatekeeper::StopMovement()
{
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->MaxWalkSpeed = 0;
	Attacking = true;
}


GatekeeperState AGatekeeper::GetState()
{
	return GKState;
}

void AGatekeeper::SetState(GatekeeperState state)
{
	GKState = state;
	BehaviourStateEvent();
}

void AGatekeeper::BehaviourStateEvent()
{
	switch (GKState)
	{
	case GatekeeperState::Start:
		if (BossStartPoint)
			AIController->MoveToActor(BossStartPoint, 5);
		break;
	case GatekeeperState::Chase:
		AIController->MoveToActor(AttackTarget, ReachTargetDistance);
		break;
	case GatekeeperState::HeavyAttack:
		AIController->MoveToActor(AttackTarget, ReachTargetDistance);
		break;
	case GatekeeperState::SummonPortals:
		Attacking = false;
		if (PortalReset)
		{
			PortalReset = false;
			GetMesh()->GetAnimInstance()->Montage_Play(SummonPortalMontage);
			GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, SummonPortalMontage);

			StopMovement();
		}
		break;
	case GatekeeperState::Staggered:

		break;
	case GatekeeperState::Dead:

		break;
	}

}

void AGatekeeper::Tick(float DeltaTime)
{
	if (Attacking)
		TrackPlayer();
}

void AGatekeeper::OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (Montage == BaseAttackMontage)
		{
			Attacking = false;
			SetSpeed();
			AttackReset = true;
			SetState(GatekeeperState::HeavyAttack);
		}
		else if ((Montage == HeavyAttackMontage || Montage == StompMontage || Montage == BeamMontage) && GKState == GatekeeperState::HeavyAttack)
		{
			Attacking = false;
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle,
				this,
				&AGatekeeper::AttackDelay,
				FMath::RandRange(GetMinAttackDelay(), GetMaxAttackDelay()));
		}
		else if (Montage == SummonPortalMontage)
		{
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(TimerHandle,
				this,
				&AGatekeeper::PortalDelay,
				1.0f);
			
		}
		else if (Montage == BeamMontage && GKState == GatekeeperState::SummonPortals)
		{
			SetSpeed();
			AttackReset = true;
			HeavyReset = true;
			PortalReset = true;
			SetState(GatekeeperState::Chase);
		}
		else if (Montage == EnrageMontage)
		{
			SetSpeed();
			SetState(GatekeeperState::HeavyAttack);
		}
	}
}

void AGatekeeper::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (Result.IsSuccess())
	{
		switch (GKState)
		{
		case GatekeeperState::Chase:
			if (AttackReset)
			{
				AttackReset = false;
				GetMesh()->GetAnimInstance()->Montage_Play(BaseAttackMontage);
				GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, BaseAttackMontage);
				StopMovement();
				TrackPlayer();
			}
			break;
		case GatekeeperState::HeavyAttack:
			if (HeavyReset)
			{
				HeavyReset = false;
				RandomMontage = MontageArray[FMath::RandRange(0, MontageArray.Num() - 1)];
				GetMesh()->GetAnimInstance()->Montage_Play(RandomMontage);
				GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, RandomMontage);
				StopMovement();
				TrackPlayer();
			}
			break;
		}
	}
}

void AGatekeeper::SetSpeed()
{
	if (Enraged)
	{
		GetCharacterMovement()->MaxWalkSpeed = GetRunSpeed();
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = GetWalkSpeed();
	}
}

void AGatekeeper::AttackDelay()
{
	HeavyReset = true;
	SetSpeed();
	SetState(GatekeeperState::Chase);
}

void AGatekeeper::PortalDelay()
{
	if (GetHealth() <= GetMaxHealth() * GetHPThresholdLow())
	{
		AttackReset = true;
		HeavyReset = true;
		Enrage();
	}
	else
	{
		GetMesh()->GetAnimInstance()->Montage_Play(BeamMontage);
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, BeamMontage);
	}
}

void AGatekeeper::Enrage()
{
	if (!Enraged)
	{
		Enraged = true;
		SetAttack(GetAttack() * GetAttackMultiplier());
		GetMesh()->GetAnimInstance()->Montage_Play(EnrageMontage);
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, EnrageMontage);
	}
}

void AGatekeeper::TrackPlayer()
{
	float zLook = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AttackTarget->GetActorLocation()).Yaw;
	SetActorRotation(UKismetMathLibrary::RLerp(FRotator(0, GetActorRotation().Yaw, 0), FRotator(0, zLook, 0), 0.02f, true));
	
}

void AGatekeeper::SpawnPortals(int PortalCount)
{
	FActorSpawnParameters SpawnInfo;
	for (int i = 0; i < PortalCount; i++)
	{
		GetWorld()->SpawnActor<APortalSpawn>(PortalSpawns[i]->GetActorLocation(), FRotator(PortalSpawns[i]->GetActorRotation()), SpawnInfo);
	}
}

void AGatekeeper::Death()
{
	//Set HP Widget Component Not Visible

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->MaxWalkSpeed = 0;
	Attacking = false;
	SetState(GatekeeperState::Dead);

	if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		GetMesh()->GetAnimInstance()->StopAllMontages(false);
	}

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	SetLifeSpan(10);
}

int AGatekeeper::HealthCheck(float Damage)
{
	if (HPThresholdCheck(GetHPThresholdHigh(), Damage))
	{
		UpdateHealth(true, Damage);
		return 1;
	}
	else if (HPThresholdCheck(GetHPThresholdMed(), Damage))
	{
		UpdateHealth(true, Damage);
		return 2;
	}
	else if (HPThresholdCheck(GetHPThresholdLow(), Damage))
	{
		UpdateHealth(true, Damage);
		return 3;
	}
	else
	{
		UpdateHealth(false, Damage);
		return 0;
	}
}

bool AGatekeeper::HPThresholdCheck(float HPThreshold, float Damage)
{
	return GetHealth() > GetMaxHealth() * HPThreshold && GetHealth() - Damage <= GetMaxHealth() * HPThreshold;
}

void AGatekeeper::UpdateHealth(bool StopMovement, float Damage)
{
	SetHealth(GetHealth() - Damage);

	if (StopMovement)
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->MaxWalkSpeed = 0;
	}

	//Update Health Bar Widget

}

void AGatekeeper::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	float PortalCount = HealthCheck(FMath::Floor(Damage * (25 / (25 + GetDefense()))));

	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, FString::FromInt((int)PortalCount));

	if (PortalCount > 0)
	{
		SpawnPortals(PortalCount);
		SetState(GatekeeperState::SummonPortals);
	}
	else
	{
		if (GetHealth() <= 0)
			Death();
	}
}

void AGatekeeper::AttackTrace(UAnimMontage* AnimTrigger)
{
	FVector StartLocation;
	FVector EndLocation;
	float AttackRadius;

	if (AnimTrigger == StompMontage)
	{
		StartLocation = GetMesh()->GetComponentLocation();
		EndLocation = StartLocation;
		AttackRadius = StompRadius;
	}
	else
	{
		return;
	}

	TArray<AActor*> ActorsToIgnore = {};
	TArray<FHitResult> OutHits;
	if (UKismetSystemLibrary::SphereTraceMulti(GetWorld(), StartLocation, EndLocation, AttackRadius,
		TraceTypeQuery3, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, OutHits, true, 
		FLinearColor::Red, FLinearColor::Green, 1.0f))
	{
		float Damage = FMath::Floor(GetAttack() * FMath::RandRange(0.9f, 1.1f));
		FDamageEvent DamageEvent;
		OutHits[0].GetActor()->TakeDamage(Damage, DamageEvent, NULL, this);
	}
}








