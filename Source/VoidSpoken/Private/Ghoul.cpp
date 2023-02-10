// Fill out your copyright notice in the Description page of Project Settings.


#include "Ghoul.h"

AGhoul::AGhoul()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = GetCapsuleComponent();
	GetMesh()->SetupAttachment(GetCapsuleComponent());

	//Initialize General Stats
	if (GetAttack() <= 0)
		SetAttack(3);
	Stats->Defense = 10;
	Stats->SetMaxHealth(30);
	Stats->Health = Stats->GetMaxHealth();

	//Initialize Character Movement Stats
	SetWalkSpeed(GhoulWalkSpeed);
	SetRunSpeed(GhoulRunSpeed);
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
	if (!LockState || state == EGhoulState::Dead)
	{
		GhState = state;
		SetSpeed();
		BehaviourStateEvent();
	}
}

void AGhoul::BehaviourStateEvent()
{
	switch (GhState)
	{
	case EGhoulState::Idle:
		bCanAttack = false;
		GetWorldTimerManager().SetTimer(TimerHandle,
			this,
			&AGhoul::IdleDelay,
			FMath::RandRange(IdleTime * 0.8f, IdleTime * 1.2f));
		break;

	case EGhoulState::Patrol:
		bCanAttack = false;
		if (PatrolPoints.Num() > 1 && TestPathExists(PatrolPoints[PatrolIndex]))
			AIController->MoveToActor(PatrolPoints[PatrolIndex], MeleeTargetDistance);
		else if (PatrolPoints.Num() == 1 && TestPathExists(PatrolPoints[0]) && FVector::Distance(GetActorLocation(), PatrolPoints[0]->GetActorLocation()) > MeleeTargetDistance)
			AIController->MoveToActor(PatrolPoints[0], MeleeTargetDistance);
		break;

	case EGhoulState::CallAllies:
		bCanAttack = false;
		GetMesh()->GetAnimInstance()->Montage_Play(ScreechMontage);
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, ScreechMontage);
		break;

	case EGhoulState::Chase:
		bCanAttack = true;
		if (TestPathExists(AttackTarget))
			AIController->MoveToActor(AttackTarget, ReachTargetDistance);
		break;

	case EGhoulState::Attack:
		bCanAttack = false;
		if (TestPathExists(AttackTarget))
			AIController->MoveToActor(AttackTarget, ReachTargetDistance);
		else if (AIController->LineOfSightTo(AttackTarget) || FVector::Distance(GetActorLocation(), AttackTarget->GetActorLocation()) <= ReachTargetDistance)
			BeginAttack();
		else 
			SetState(EGhoulState::AttackCooldown);
		break;

	case EGhoulState::AttackCooldown:
		bCanAttack = true;
		AttackCooldown();
		break;

	case EGhoulState::CirclePlayer:
		bCanAttack = true;
		CirclePlayer();
		break;

	case EGhoulState::CombatIdle:
		bCanAttack = true;
		CombatIdle();
		break;

	case EGhoulState::Staggered:
		bCanAttack = false;
		LockState = true;
		if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
			GetMesh()->GetAnimInstance()->StopAllMontages(false);
		if (StaggerMontage)
		{
			GetMesh()->GetAnimInstance()->Montage_Play(StaggerMontage);
			GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, StaggerMontage);
		}
		StopMovement();
		break;

	case EGhoulState::Dead:
		bCanAttack = false;
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
	else if (Montage == Attack3Montage)
	{

	}
}

void AGhoul::SetAttackingRight(bool right)
{
	AttackingRight = right;
}

void AGhoul::SetAttackingLeft(bool left)
{
	AttackingLeft = left;
}

void AGhoul::TriggerAttack()
{
	SetState(EGhoulState::Attack);

}

void AGhoul::BeginAttack()
{
	if (GetEnemyType() == EEnemyType::Melee)
	{
		AIController->ClearFocus(EAIFocusPriority::Gameplay);

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

		TimeOfLastAttack = UGameplayStatics::GetTimeSeconds(GetWorld());
	}
	else
	{
		if (AIController->LineOfSightTo(AttackTarget))
		{
			AIController->ClearFocus(EAIFocusPriority::Gameplay);

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

			TimeOfLastAttack = UGameplayStatics::GetTimeSeconds(GetWorld());
		}
		else
		{
			SetState(EGhoulState::AttackCooldown);
		}

	}
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
	if (!OutActors.IsEmpty())
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
	float zLook = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AttackTarget->GetActorLocation()).Yaw;
	SetActorRotation(FRotator(GetActorRotation().Pitch, zLook, GetActorRotation().Roll));

	float XVel = ProjectileSpeed;
	FVector HPosDiff = AttackTarget->GetActorLocation() + FVector(0, 0, 50) - ThrowPoint->GetComponentLocation();
	FRotator Rotation = HPosDiff.Rotation();
	float HeightDiff = HPosDiff.Z;
	HPosDiff.Z = 0;
	float HDistance = HPosDiff.Length();
	float TimeToHit = HDistance / XVel;
	float ZVel = (HeightDiff - 0.5f * GetWorld()->GetGravityZ() * TimeToHit * TimeToHit) / TimeToHit;
	float InitVel = FMath::Sqrt(XVel * XVel + ZVel * ZVel);
	float InitAngle = FMath::RadiansToDegrees(FMath::Atan(ZVel / XVel));
	Rotation.Pitch = InitAngle;

	CreateSpike(Rotation, ThrowPoint->GetComponentLocation(), true, InitVel);
}

void AGhoul::CreateSpike(FRotator Rotation, FVector Location, bool UseSpikeCollision, float InitVel)
{
	FActorSpawnParameters SpawnInfo;
	ASpikeProjectile* CreatedSpike = GetWorld()->SpawnActor<ASpikeProjectile>(Spike, Location, Rotation, SpawnInfo);
	if (UseSpikeCollision)
	{
		CreatedSpike->SetDamage(Attack);
		CreatedSpike->SetSpeed(InitVel);
		CreatedSpike->ProjectileMovementComponent->ProjectileGravityScale = 1.0f;
		CreatedSpike->Owner = this;
	}
	else
		CreatedSpike->SetDamage(0, false, 0.25f);
}

void AGhoul::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	if (Result.IsSuccess())
	{
		switch (GhState)
		{
		case (EGhoulState::Chase):
			SetState(EGhoulState::AttackCooldown);
			break;

		case EGhoulState::Attack:
			BeginAttack();
			break;

		case EGhoulState::AttackCooldown:
			GetWorldTimerManager().SetTimer(PatrolTimerHandle,
				this,
				&AGhoul::AttackCooldown,
				1);
			break;

		case EGhoulState::CirclePlayer:
			GetCharacterMovement()->MaxWalkSpeed = BackUpSpeed;
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
		AIController->SetFocus(AttackTarget);

		if (MontageArray.Contains(Montage) || Montage == RangedAttackMontage || Montage == BurstMontage)
		{
			if (GhState == EGhoulState::Attack)
				SetState(EGhoulState::AttackCooldown);
		}
		else if (Montage == StaggerMontage)
		{
			LockState = false;
			SetState(EGhoulState::Chase);
		}
		else if (Montage == ScreechMontage)
		{
			EnterCombat(UGameplayStatics::GetPlayerPawn(GetWorld(), 0), false);

			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Camera));
			UClass* SeekClass = ABaseEnemy::StaticClass();
			TArray<AActor*> ActorsToIgnore = { this };
			TArray<AActor*> OutActors;
			UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), CallAlliesRange, ObjectTypes, SeekClass, ActorsToIgnore, OutActors);

			if (!OutActors.IsEmpty())
			{
				for (int i = 0; i < OutActors.Num(); i++)
				{
					if (Cast<ABaseEnemy>(OutActors[i]))
					{
						if (AIController->LineOfSightTo(Cast<ACharacter>(OutActors[i])))
							Cast<ABaseEnemy>(OutActors[i])->EnterCombat(AttackTarget, false);
					}
				}
			}
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

void AGhoul::EnterCombat(APawn* OtherPawn, bool Cooldown)
{
	AttackTarget = OtherPawn;
	bInCombat = true;
	GetWorldTimerManager().SetTimer(PatrolTimerHandle,
		this,
		&AGhoul::CheckPatrolReset,
		PTHandleInterval);

	UpdateHealthBar.Broadcast();

	SetSpeed();

	if (StaggerComponent->bCanGainStagger)
	{
		AIController->SeePlayer(AttackTarget);

		if (Cooldown)
			SetState(EGhoulState::AttackCooldown);
		else
			SetState(EGhoulState::Chase);
	}

	PawnSensing->SetSensingUpdatesEnabled(false);
}

void AGhoul::CirclePlayer()
{
	if (AttackTarget && GetState() == EGhoulState::CirclePlayer)
	{
		if (GetEnemyType() == EEnemyType::Melee)
		{
			FVector PlayerToEnemy = GetActorLocation() - AttackTarget->GetActorLocation();
			PlayerToEnemy.Z = 0;

			if (PlayerToEnemy.Length() > CircleTargetDistance)
				GetCharacterMovement()->MaxWalkSpeed = GetRunSpeed();
			else
				GetCharacterMovement()->MaxWalkSpeed = BackUpSpeed;

			FRotator Direction = PlayerToEnemy.Rotation();
			if (!MoveRight)
				Direction.Yaw += 30;
			else
				Direction.Yaw -= 30;

			FVector TargetPosition;
			TargetPosition = AttackTarget->GetActorLocation() + Direction.Vector() * CircleTargetDistance;

			if (TestPathExists(TargetPosition))
				AIController->MoveToLocation(TargetPosition, MeleeTargetDistance);
			else
				MoveRight = !MoveRight;
		}
		else if (FVector::Distance(GetActorLocation(), AttackTarget->GetActorLocation()) > ReachTargetDistance)
		{
			if (TestPathExists(AttackTarget))
				AIController->MoveToActor(AttackTarget, ReachTargetDistance);
		}
	
		GetWorldTimerManager().SetTimer(AttackCooldownTimer,
			this,
			&AGhoul::CirclePlayer,
			2);
	}
}

void AGhoul::CombatIdle()
{
	if (AttackTarget && GetState() == EGhoulState::CombatIdle)
	{
		if (GetEnemyType() == EEnemyType::Melee)
		{
			if (FVector::Distance(GetActorLocation(), AttackTarget->GetActorLocation()) > MeleeSpreadRange)
			{
				if (TestPathExists(AttackTarget))
					AIController->MoveToActor(AttackTarget, MeleeSpreadRange);
			}
		}
		else 
		{
			float DistToPlayer = FVector::Distance(AttackTarget->GetActorLocation(), GetActorLocation());
			if (DistToPlayer > ReachTargetDistance)
			{
				GetCharacterMovement()->MaxWalkSpeed = GetRunSpeed();

				if (TestPathExists(AttackTarget))
					AIController->MoveToActor(AttackTarget, ReachTargetDistance);
			}
			else if (DistToPlayer < BackOffRange)
			{
				FVector3d BackUpTargetPos = GetActorLocation() - AttackTarget->GetActorLocation();
				BackUpTargetPos.Z = 0;
				BackUpTargetPos = GetActorLocation() + BackUpTargetPos.GetSafeNormal() * BackUpDistance;
				GetCharacterMovement()->MaxWalkSpeed = BackUpSpeed;

				AIController->MoveToLocation(BackUpTargetPos, 0);
			}
		}

		GetWorldTimerManager().SetTimer(AttackCooldownTimer,
			this,
			&AGhoul::CirclePlayer,
			2);
	}
}

void AGhoul::OnSeePawn(APawn* OtherPawn)
{
	AttackTarget = OtherPawn;
	bInCombat = true;
	GetWorldTimerManager().SetTimer(PatrolTimerHandle,
		this,
		&AGhoul::CheckPatrolReset,
		PTHandleInterval);
	PawnSensing->SetSensingUpdatesEnabled(false);
	UpdateHealthBar.Broadcast();

	SetState(EGhoulState::CallAllies);
}

void AGhoul::OnStaggered()
{
	Super::OnStaggered();

	SetState(EGhoulState::Staggered);
}

bool AGhoul::CheckLineOfSight(AActor* OtherActor)
{
	return AIController->LineOfSightTo(OtherActor) && FVector::Distance(GetActorLocation(), OtherActor->GetActorLocation()) <= PatrolResetDistance;
}

void AGhoul::SetCombatIdle()
{
	if (GetState() != EGhoulState::Attack && GetState() != EGhoulState::CirclePlayer)
		SetState(EGhoulState::CombatIdle);
}

void AGhoul::SetCirclePlayer(bool RandomizeDirection, float AdditionalDistance)
{
	if (GetState() != EGhoulState::Attack && GetState() != EGhoulState::CirclePlayer && GetState() != EGhoulState::CallAllies)
	{
		if (RandomizeDirection)
		{
			int Random = FMath::RandRange(0, 1);

			switch (Random)
			{
			case 0:
				MoveRight = true;
				break;
			case 1:
				MoveRight = false;
				break;
			}
		}

		CircleTargetDistance = MeleeSpreadRange + AdditionalDistance;
		SetState(EGhoulState::CirclePlayer);

	}
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

	if (GetEnemyType() == EEnemyType::Melee)
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

	SetState(EGhoulState::Idle);
}

void AGhoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AttackTarget)
	{
		if (!GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() && !AIController->IsFollowingAPath() && GetState() != EGhoulState::Dead)
		{
			if (!GetWorldTimerManager().IsTimerActive(TimerHandle) && GhState != EGhoulState::AttackCooldown)
			{
				BehaviourStateEvent();
			}
		}
	}
}

void AGhoul::SetSpeed()
{
	if (GetState() == EGhoulState::Idle || GetState() == EGhoulState::Patrol || GetState() == EGhoulState::AttackCooldown || GetState() == EGhoulState::CirclePlayer || GetState() == EGhoulState::CombatIdle)
		GetCharacterMovement()->MaxWalkSpeed = GetWalkSpeed();
	else if (GetState() == EGhoulState::Attack || GetState() == EGhoulState::Chase)
		GetCharacterMovement()->MaxWalkSpeed = GetRunSpeed();
	else
		GetCharacterMovement()->MaxWalkSpeed = 0;
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
	bIsDead = true;
	bInCombat = false;
	AIController->ClearFocus(EAIFocusPriority::Gameplay);

	if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		GetMesh()->GetAnimInstance()->StopAllMontages(false);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetLifeSpan(10);
}

void AGhoul::AttackCooldown()
{
	if (AttackTarget)
	{
		if (GetEnemyType() == EEnemyType::Ranged)
		{
			float DistToPlayer = FVector::Distance(AttackTarget->GetActorLocation(), GetActorLocation());
			if (DistToPlayer <= BackOffRange)
			{
				FVector3d BackUpTargetPos = GetActorLocation() - AttackTarget->GetActorLocation();
				BackUpTargetPos.Z = 0;
				BackUpTargetPos = GetActorLocation() + BackUpTargetPos.GetSafeNormal() * BackUpDistance;
				GetCharacterMovement()->MaxWalkSpeed = BackUpSpeed;

				AIController->MoveToLocation(BackUpTargetPos, 0);
			}
			else if (DistToPlayer > ReachTargetDistance)
			{
				SetSpeed();
				if (TestPathExists(AttackTarget))
					AIController->MoveToActor(AttackTarget, ReachTargetDistance);
			}
		}
		else
		{
			if (FVector::Distance(GetActorLocation(), AttackTarget->GetActorLocation()) > MeleeSpreadRange)
			{
				AIController->MoveToActor(AttackTarget, MeleeSpreadRange);
			}
		}
	}
}


void AGhoul::CheckPatrolReset()
{
	if (bInCombat && AttackTarget)
	{
		if ((GhState == EGhoulState::Attack || GhState == EGhoulState::AttackCooldown) && AttackTarget)
		{
			if (!AIController->LineOfSightTo(AttackTarget) || !TestPathExists(AttackTarget))
			{
				PResetTimer += PTHandleInterval;
			}
			else
			{
				PResetTimer = 0;
			}

			if (PResetTimer >= PatrolResetTime)
			{
				PResetTimer = 0;
				PatrolReset();
			}
		}
		else
		{
			PResetTimer = 0;
		}

		GetWorldTimerManager().SetTimer(PatrolTimerHandle,
			this,
			&AGhoul::CheckPatrolReset,
			PTHandleInterval);
	}
}

void AGhoul::PatrolReset()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);

	AttackTarget = nullptr;
	bInCombat = false;
	AIController->ClearFocus(EAIFocusPriority::Gameplay);

	Stats->Health = Stats->GetMaxHealth();
	UpdateHealthBar.Broadcast();

	SetState(EGhoulState::Idle);

	PawnSensing->SetSensingUpdatesEnabled(true);
}

bool AGhoul::TestPathExists(AActor* Target)
{
	const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	const ANavigationData* NavData = NavSys->GetNavDataForProps(GetNavAgentPropertiesRef());
	if (NavData && Target)
	{
		FPathFindingQuery MyAIQuery = FPathFindingQuery(this, *NavData, GetActorLocation(), Target->GetActorLocation());
		return NavSys->TestPathSync(MyAIQuery);
	}
	
	return false;
}

bool AGhoul::TestPathExists(FVector Target)
{
	UNavigationSystemV1* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	const ANavigationData* NavData = NavSys->GetNavDataForProps(GetNavAgentPropertiesRef());
	if (NavData)
	{
		FPathFindingQuery MyAIQuery(this, *NavData, GetActorLocation(), Target);
		return NavSys->TestPathSync(MyAIQuery, EPathFindingMode::Hierarchical);
	}

	return false;
}

void AGhoul::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	Super::TakeAnyDamage(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);

	if (!AttackTarget)
		OnSeePawn(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	UpdateHealthBar.Broadcast();

	if (Stats->Health <= 0)
		Death();
}
