// Fill out your copyright notice in the Description page of Project Settings.


#include "Gatekeeper.h"

AGatekeeper::AGatekeeper()
{
	if (CapsuleComponent)
	{
		RootComponent = CapsuleComponent;
		CapsuleComponent->SetRelativeScale3D(FVector(2.5, 2.5, 2.5));
		CapsuleComponent->SetCapsuleHalfHeight(88);
		CapsuleComponent->SetCapsuleRadius(34);
	}

	if (!GetMesh())
	{
		static ConstructorHelpers::FObjectFinder<USkeletalMesh>M(TEXT("/Game/Blueprints/Bosses/Gatekeeper/Components/Standing_Idle.Standing_Idle"));

		if (M.Succeeded())
		{
			GetMesh()->SetSkeletalMesh(M.Object);
		}

		GetMesh()->SetupAttachment(CapsuleComponent);

		GetMesh()->SetRelativeLocation(FVector(0, 0, -90));
		GetMesh()->SetRelativeRotation(FRotator(0, 0, -90));
	}

	if (!Weapon)
	{
		Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon"));
		static ConstructorHelpers::FObjectFinder<USkeletalMesh>M(TEXT("/Game/Assets/Weapon_Pack/Skeletal_Mesh/SK_GreatHammer.SK_GreatHammer"));

		if (M.Succeeded())
		{
			Weapon->SetSkeletalMesh(M.Object);
		}

		Weapon->SetupAttachment(GetMesh(), TEXT("RightHand"));

		Weapon->SetRelativeLocation(FVector(-5.688907, -9.429593, 1.854035));
		Weapon->SetRelativeRotation(FRotator(-9.846553, 88.246216, -84.398694));
	}

	if (!WeaponCollider)
	{
		WeaponCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Weapon Collider"));
		WeaponCollider->SetupAttachment(Weapon);

		WeaponCollider->SetRelativeLocation(FVector(-26, 0.5, 95));
		WeaponCollider->SetSphereRadius(WeaponRadius);
	}

	if (!ChestLocation)
	{
		ChestLocation = CreateDefaultSubobject<USphereComponent>(TEXT("Chest Collider"));
		ChestLocation->SetupAttachment(GetMesh(), TEXT("Spine2"));

		ChestLocation->SetRelativeLocation(FVector(0, 0, 19));
		ChestLocation->SetRelativeRotation(FRotator(15, 0, 0));
		ChestLocation->SetSphereRadius(ChestRadius);
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
	GetCharacterMovement()->MaxWalkSpeed = GetWalkSpeed();

	if (BossStartPoint == FVector(0, 0, 0))
		BossStartPoint = GetActorLocation() + GetActorForwardVector() * 300;
	
	MontageEndDelegate.BindUObject(this, &AGatekeeper::OnAnimationEnded);
	if (BaseAttackMontage)
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, BaseAttackMontage);
	if (HeavyAttackMontage)
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, HeavyAttackMontage);
	if (StompMontage)
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, StompMontage);
	if (BeamMontage)
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, BeamMontage);
	if (SummonPortalMontage)
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, SummonPortalMontage);
	if (EnrageMontage)
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, EnrageMontage);

	if (PawnSensing)
	{
		PawnSensing->SightRadius = 3500.0f;
		PawnSensing->SetPeripheralVisionAngle(85.0f);
		PawnSensing->bOnlySensePlayers = true;
		PawnSensing->OnSeePawn.AddDynamic(this, &AGatekeeper::OnSeePawn);
	}

	OnTakeAnyDamage.AddDynamic(this, &AGatekeeper::TakeAnyDamage);
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
	PlayerPawn = OtherPawn;
	AIController->SeePlayer(PlayerPawn);

	SetSpeed();

	//Set HP Widget Component Visible

	BehaviourChange(GatekeeperState::Chase);
}

void AGatekeeper::BeginPlay()
{
	Super::BeginPlay();

	BehaviourChange(GatekeeperState::Start);
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
		AIController->MoveToLocation(BossStartPoint);
		break;
	case GatekeeperState::Chase:
		if (AIController->MoveToActor(PlayerPawn, ReachTargetDistance))
		{
			if (AttackReset)
			{
				AttackReset = false;
				GetMesh()->GetAnimInstance()->Montage_Play(BaseAttackMontage);
				StopMovement();
				TrackPlayer();
			}
		}
		break;
	case GatekeeperState::HeavyAttack:
		if (AIController->MoveToActor(PlayerPawn, ReachTargetDistance))
		{
			if (HeavyReset)
			{
				HeavyReset = false;
				RandomMontage = MontageArray[FMath::RandRange(0, MontageArray.Num() - 1)];
				GetMesh()->GetAnimInstance()->Montage_Play(RandomMontage);
				StopMovement();
				TrackPlayer();
			}
		}
		break;
	case GatekeeperState::SummonPortals:
		Attacking = false;
		if (PortalReset)
		{
			PortalReset = false;
			GetMesh()->GetAnimInstance()->Montage_Play(SummonPortalMontage);
			StopMovement();
		}
		break;
	case GatekeeperState::Staggered:

		break;
	case GatekeeperState::Dead:

		break;
	}

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
			BehaviourChange(HeavyAttack);
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
				&AGatekeeper::AttackDelay,
				1.0f);
			if (GetHealth() <= GetMaxHealth() * GetHPThresholdLow())
			{
				AttackReset = true;
				HeavyReset = true;
				Enrage();
			}
			else
			{
				GetMesh()->GetAnimInstance()->Montage_Play(BeamMontage);
			}
		}
		else if (Montage == BeamMontage && GKState == GatekeeperState::SummonPortals)
		{
			SetSpeed();
			AttackReset = true;
			HeavyReset = true;
			PortalReset = true;
			BehaviourChange(Chase);
		}
		else if (Montage == EnrageMontage)
		{
			SetSpeed();
			BehaviourChange(HeavyAttack);
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
	BehaviourChange(Chase);
}

void AGatekeeper::Enrage()
{
	if (!Enraged)
	{
		Enraged = true;
		SetAttack(GetAttack() * GetAttackMultiplier());
		GetMesh()->GetAnimInstance()->Montage_Play(EnrageMontage);
	}
}

void AGatekeeper::TrackPlayer()
{
	float zLook = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), PlayerPawn->GetActorLocation()).Yaw;
	SetActorRotation(UKismetMathLibrary::RLerp(FRotator(0, 0, GetActorRotation().Yaw), FRotator(0, 0, zLook), 0.02f, true));
	if (Attacking)
		TrackPlayer();
}

void AGatekeeper::SpawnPortals(int PortalCount)
{
	FActorSpawnParameters SpawnInfo;
	GetWorld()->SpawnActor<APortalSpawn>(PortalSpawns[0]->GetActorLocation(), FRotator(PortalSpawns[0]->GetActorRotation()), SpawnInfo);

	if (PortalCount > 1)
	{
		GetWorld()->SpawnActor<APortalSpawn>(PortalSpawns[1]->GetActorLocation(), FRotator(PortalSpawns[1]->GetActorRotation()), SpawnInfo);
	}

	if (PortalCount > 2)
	{
		GetWorld()->SpawnActor<APortalSpawn>(PortalSpawns[2]->GetActorLocation(), FRotator(PortalSpawns[2]->GetActorRotation()), SpawnInfo);
	}
}

void AGatekeeper::Death()
{
	//Set HP Widget Component Not Visible

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->MaxWalkSpeed = 0;
	Attacking = false;
	BehaviourChange(Dead);

	if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		GetMesh()->GetAnimInstance()->StopAllMontages(false);
	}

	CapsuleComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
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
	float PortalCount = HealthCheck(FMath::Floor(Damage * GetDamageMultiplier() / GetDefense()));

	if (PortalCount > 0)
	{
		SpawnPortals(PortalCount);
		BehaviourChange(SummonPortals);
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

	if (AnimTrigger == BaseAttackMontage || AnimTrigger == HeavyAttackMontage)
	{
		StartLocation = ChestLocation->GetComponentLocation();
		EndLocation = WeaponCollider->GetComponentLocation();
		AttackRadius = WeaponRadius;
	}
	else if (AnimTrigger == StompMontage)
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








