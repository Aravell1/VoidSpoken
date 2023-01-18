// Fill out your copyright notice in the Description page of Project Settings.


#include "Ghoul.h"

AGhoul::AGhoul()
{
	RootComponent = GetCapsuleComponent();
	GetMesh()->SetupAttachment(GetCapsuleComponent());

	//Initialize General Stats
	if (GetAttack() <= 0)
		SetAttack(3);
	Stats->Defense = 10;
	Stats->SetMaxHealth(15);
	Stats->Health = Stats->GetMaxHealth();

	//Initialize Character Movement Stats
	SetWalkSpeed(200);
	SetRunSpeed(300);
	GetCharacterMovement()->MaxWalkSpeed = GetWalkSpeed();

	if (PawnSensing)
	{
		PawnSensing->SightRadius = 2000.0f;
		PawnSensing->SetPeripheralVisionAngle(65.0f);
		PawnSensing->bOnlySensePlayers = true;
		PawnSensing->OnSeePawn.AddDynamic(this, &AGhoul::OnSeePawn);
	}
}

EGhoulState AGhoul::GetState()
{
	return GhState;
}

void AGhoul::SetState(EGhoulState state)
{
	if (!LockState)
	{
		GhState = state;
		SetSpeed();
		BehaviourStateEvent();
	}
}

EGhoulType AGhoul::GetType()
{
	return GhType;
}

void AGhoul::BehaviourStateEvent()
{
	switch (GhState)
	{
	case EGhoulState::Idle:
		GetWorldTimerManager().SetTimer(TimerHandle,
			this,
			&AGhoul::IdleDelay,
			FMath::RandRange(IdleTime * 0.8f, IdleTime * 1.2f));
		break;
	case EGhoulState::Patrol:
		if (PatrolPoints.Num() > 1)
			AIController->MoveToActor(PatrolPoints[PatrolIndex], MeleeTargetDistance);
		break;
	case EGhoulState::Attack:		
		AIController->MoveToActor(AttackTarget, ReachTargetDistance);
		break;
	case EGhoulState::AttackCooldown:
		if (GetType() == EGhoulType::Melee || FVector::Distance(GetActorLocation(), AttackTarget->GetActorLocation()) <= BackOffRange)
		{
			FVector3d BackUpTargetPos = GetActorLocation() - AttackTarget->GetActorLocation();
			BackUpTargetPos.Z = 0;
			BackUpTargetPos = GetActorLocation() + BackUpTargetPos.GetSafeNormal() * BackUpDistance;

			AIController->MoveToLocation(BackUpTargetPos, 0);
		}

		GetWorldTimerManager().SetTimer(TimerHandle,
			this,
			&AGhoul::AttackDelay,
			FMath::RandRange(AttackCD * 0.8f, AttackCD * 1.2f));
		break;
	case EGhoulState::Dead:
		LockState = true;
		break;
	}
}

void AGhoul::SetAttacking(UAnimMontage* Montage, bool Attacking)
{
	if (Montage == Attack1Montage)
	{
		AttackingLeft = Attacking;
	}
	else if (Montage == Attack2Montage)
	{
		AttackingRight = Attacking;
	}
	/*else if (Montage == Attack3Montage)
	{

	}*/
}

void AGhoul::SetAttackingRight(bool right)
{
	AttackingRight = right;
}

void AGhoul::SetAttackingLeft(bool left)
{
	AttackingLeft = left;
}

void AGhoul::TriggerSpikes(UAnimMontage* Montage)
{
	if (Montage == RangedAttackMontage)
	{
		SpikeThrow();
	}
	else if (Montage == BurstMontage)
	{
		SpikeBurst();
	}
}

void AGhoul::SpikeBurst()
{
	FRotator Rotation = GetActorRotation();
	Rotation.Pitch += 90;
	Rotation.Yaw += FMath::RandRange(0.0f, 45.0f);
	FVector SpawnLocation = HeadLocation->GetComponentLocation() + Rotation.Vector() * BurstSpikeSpawnDistance;

	CreateSpike(Rotation, SpawnLocation, false);

	for (int i = 0; i < 3; i++)
	{
		Rotation.Pitch -= 45;
		SpawnLocation = HeadLocation->GetComponentLocation() + Rotation.Vector() * BurstSpikeSpawnDistance;

		for (int j = 0; j < 8; j++)
		{
			CreateSpike(Rotation, SpawnLocation, false);

			if (i == 1)
			{
				Rotation.Yaw -= 45;
				SpawnLocation = HeadLocation->GetComponentLocation() + Rotation.Vector() * BurstSpikeSpawnDistance;
			}
			else
			{
				Rotation.Yaw -= 90;
				SpawnLocation = HeadLocation->GetComponentLocation() + Rotation.Vector() * BurstSpikeSpawnDistance;
				j++;
			}
		}
	}

	TArray<AActor*> ActorsToIgnore = { GetOwner() };
	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Camera));

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), HeadLocation->GetComponentLocation(), BurstRadius, ObjectTypes, nullptr, ActorsToIgnore, OutActors);
	if (OutActors.Num() > 0)
	{
		float Damage = FMath::Floor(GetAttack() * FMath::RandRange(1.8f, 2.2f));
		for (int i = 0; i < OutActors.Num(); i++)
		{
			if (OutActors[i] == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
			{
				UGameplayStatics::ApplyDamage(OutActors[i], Damage, NULL, this, NULL);
				return;
			}
		}
	}
}

void AGhoul::SpikeThrow()
{
	FRotator Rotation = (AttackTarget->GetActorLocation() - ThrowPoint->GetComponentLocation()).Rotation();
	Rotation.Pitch += 2;
	CreateSpike(Rotation, ThrowPoint->GetComponentLocation(), true);
}

void AGhoul::CreateSpike(FRotator Rotation, FVector Location, bool UseSpikeCollision)
{
	FActorSpawnParameters SpawnInfo;
	ASpikeProjectile* CreatedSpike = GetWorld()->SpawnActor<ASpikeProjectile>(Spike, Location, Rotation, SpawnInfo);
	if (UseSpikeCollision)
		CreatedSpike->SetDamage(Attack);
	else
		CreatedSpike->SetDamage(0, false, 0.25f);
}

void AGhoul::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (Result.IsSuccess())
	{
		switch (GhState)
		{
		case EGhoulState::Attack:
			if (GhType == EGhoulType::Melee)
			{
				if (AttackReset)
				{
					AttackReset = false;
					if (Stats->Health <= Stats->GetMaxHealth() * 0.5f)
					{
						GetMesh()->GetAnimInstance()->Montage_Play(BurstMontage);
						GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, BurstMontage);
					}
					else
					{
						RandomMontage = MontageArray[FMath::RandRange(0, MontageArray.Num() - 1)];
						GetMesh()->GetAnimInstance()->Montage_Play(RandomMontage);
						GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, RandomMontage);
					}
					StopMovement();
				}
			}
			else
			{
				if (AttackReset)
				{
					AttackReset = false;
					if (FVector::Distance(GetActorLocation(), AttackTarget->GetActorLocation()) <= BurstRadius)
					{
						GetMesh()->GetAnimInstance()->Montage_Play(BurstMontage);
						GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, BurstMontage);
					}
					else
					{
						GetMesh()->GetAnimInstance()->Montage_Play(RangedAttackMontage);
						GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, RangedAttackMontage);
					}
					StopMovement();
				}
			}
			break;

		case EGhoulState::Patrol:
			if (PatrolIndex < PatrolPoints.Num() - 1)
				PatrolIndex++;
			else
				PatrolIndex = 0;

			SetState(EGhoulState::Idle);
			break;
		}
	}
}

void AGhoul::OnAnimationEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (MontageArray.Contains(Montage) || Montage == RangedAttackMontage || Montage == BurstMontage)
		{
			SetState(EGhoulState::AttackCooldown);
		}
	}
}

void AGhoul::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OverlappedComponent == HitBoxRight && AttackingRight) || (OverlappedComponent == HitBoxLeft && AttackingLeft))
	{
		if (OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		{
			UGameplayStatics::ApplyDamage(OtherActor, GetAttack(), NULL, this, NULL);
		}
	}
}

void AGhoul::OnSeePawn(APawn* OtherPawn)
{
	AttackTarget = OtherPawn;
	AIController->SeePlayer(AttackTarget);

	SetSpeed();

	SetState(EGhoulState::Attack);

	PawnSensing->SetSensingUpdatesEnabled(false);
}

void AGhoul::BeginPlay()
{
	Super::BeginPlay();

	if (!AIController)
		AIController = GetController<AGhoulAIController>();
	if (AIController)
		AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AGhoul::OnMoveCompleted);

	MontageEndDelegate.BindUObject(this, &AGhoul::OnAnimationEnded);
	if (Attack1Montage)
		MontageArray.Add(Attack1Montage);
	if (Attack2Montage)
		MontageArray.Add(Attack2Montage);
	if (Attack3Montage)
		MontageArray.Add(Attack3Montage);

	if (GhType == EGhoulType::Melee)
		ReachTargetDistance = MeleeTargetDistance;
	else
		ReachTargetDistance = RangedTargetDistance;

	if (!HitBoxRight)
		HitBoxRight = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Hit Box Right")));
	if (!HitBoxLeft)
		HitBoxLeft = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Hit Box Left")));
	if (!ThrowPoint)
		ThrowPoint = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Throw Point")));
	if (!HeadLocation)
		HeadLocation = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("Head Location")));
	
	if (HitBoxRight)
		HitBoxRight->OnComponentBeginOverlap.AddDynamic(this, &AGhoul::OnComponentBeginOverlap);
	if (HitBoxLeft)
		HitBoxLeft->OnComponentBeginOverlap.AddDynamic(this, &AGhoul::OnComponentBeginOverlap);

	/*if (HitBoxRight)
		UE_LOG(LogTemp, Warning, TEXT("Right Box Found****************"));
	if (HitBoxLeft)
		UE_LOG(LogTemp, Warning, TEXT("Left Box Found****************"));
	if (HeadLocation)
		UE_LOG(LogTemp, Warning, TEXT("Head Box Found****************"));
	if (ThrowPoint)
		UE_LOG(LogTemp, Warning, TEXT("Throw Box Found****************"));*/

	SetState(EGhoulState::Idle);
}

void AGhoul::Tick(float DeltaTime)
{
	/*if ((GhState == EGhoulState::Attack && FVector::Distance(AttackTarget->GetActorLocation(), GetActorLocation()) > ReachTargetDistance) 
		|| (GhState == EGhoulState::Patrol && FVector::Distance(AttackTarget->GetActorLocation(), GetActorLocation()) > MeleeTargetDistance))
	{
		if (!GetWorldTimerManager().IsTimerActive(TimerHandle) && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() && !AIController->IsFollowingAPath())
		{
			BehaviourStateEvent();
		}
	}*/
}

void AGhoul::SetSpeed()
{
	if (GetState() == EGhoulState::Idle || GetState() == EGhoulState::Patrol)
		GetCharacterMovement()->MaxWalkSpeed = GetWalkSpeed();
	else if (GetState() == EGhoulState::AttackCooldown)
		GetCharacterMovement()->MaxWalkSpeed = BackUpSpeed;
	else if (GetState() == EGhoulState::Attack)
		GetCharacterMovement()->MaxWalkSpeed = GetRunSpeed();
	else
		GetCharacterMovement()->MaxWalkSpeed = 0;
}

void AGhoul::AttackDelay()
{
	AttackReset = true;
	SetSpeed();
	if (GetState() == EGhoulState::AttackCooldown)
		SetState(EGhoulState::Attack);
}

void AGhoul::IdleDelay()
{
	if (GetState() == EGhoulState::Idle && PatrolPoints.Num() > 1)
		SetState(EGhoulState::Patrol);
}

void AGhoul::StopMovement()
{
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->MaxWalkSpeed = 0;
}

void AGhoul::Death()
{
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->MaxWalkSpeed = 0;
	SetState(EGhoulState::Dead);

	if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		GetMesh()->GetAnimInstance()->StopAllMontages(false);
	}

	if (AIController->GetFocusActor())
		AIController->ClearFocus(EAIFocusPriority::Gameplay);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	SetLifeSpan(10);
}

void AGhoul::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	Super::TakeAnyDamage(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);

	UpdateHealthBar.Broadcast();

	if (Stats->Health <= 0)
		Death();
}
