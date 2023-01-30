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

void AGhoul::SetGhoulType(EGhoulType type)
{
	GhType = type;
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
		else if (GetHasLineOfSight(Cast<ACharacter>(AttackTarget)) || FVector::Distance(GetActorLocation(), AttackTarget->GetActorLocation()) <= ReachTargetDistance)
			BeginAttack();
		break;

	case EGhoulState::AttackCooldown:
		bCanAttack = true;
		AttackCooldown();

		GetWorldTimerManager().SetTimer(TimerHandle,
			this,
			&AGhoul::AttackDelay,
			FMath::RandRange(AttackCD * 0.8f, AttackCD * 1.2f));
		break;

	case EGhoulState::Staggered:
		bCanAttack = false;
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
		AttackReset = true;
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

			TimeOfLastAttack = UGameplayStatics::GetTimeSeconds(GetWorld());
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

			TimeOfLastAttack = UGameplayStatics::GetTimeSeconds(GetWorld());
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
		CreatedSpike->ProjectileMovementComponent->InitialSpeed = InitVel;
		CreatedSpike->ProjectileMovementComponent->MaxSpeed = InitVel * 3;
		CreatedSpike->ProjectileMovementComponent->ProjectileGravityScale = 1.0f;
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
			if (GhState == EGhoulState::Attack)
				SetState(EGhoulState::AttackCooldown);
		}
		else if (Montage == StaggerMontage)
		{
			AIController->SetFocus(AttackTarget);
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
						if (GetHasLineOfSight(Cast<ACharacter>(OutActors[i])))
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

void AGhoul::OnSeePawn(APawn* OtherPawn)
{
	AttackTarget = OtherPawn;
	bInCombat = true;
	PawnSensing->SetSensingUpdatesEnabled(false);
	UpdateHealthBar.Broadcast();

	SetState(EGhoulState::CallAllies);
}

void AGhoul::OnStaggered()
{
	Super::OnStaggered();

	SetState(EGhoulState::Staggered);
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

	SetState(EGhoulState::Idle);
}

void AGhoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!GetWorldTimerManager().IsTimerActive(TimerHandle) && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying() && !AIController->IsFollowingAPath() && GhState != EGhoulState::AttackCooldown)
	{
		BehaviourStateEvent();
	}

	if (!GetWorldTimerManager().IsTimerActive(PatrolTimerHandle) && 
		(GhState == EGhoulState::Attack || GhState == EGhoulState::AttackCooldown || GhState == EGhoulState::Chase || GhState == EGhoulState::CallAllies || GhState == EGhoulState::Staggered))
	{
		GetWorldTimerManager().SetTimer(PatrolTimerHandle,
			this,
			&AGhoul::CheckPatrolReset,
			PTHandleInterval);
	}
}

void AGhoul::SetSpeed()
{
	if (GetState() == EGhoulState::Idle || GetState() == EGhoulState::Patrol || GetState() == EGhoulState::AttackCooldown)
		GetCharacterMovement()->MaxWalkSpeed = GetWalkSpeed();
	else if (GetState() == EGhoulState::Attack || GetState() == EGhoulState::Chase)
		GetCharacterMovement()->MaxWalkSpeed = GetRunSpeed();
	else
		GetCharacterMovement()->MaxWalkSpeed = 0;
}

void AGhoul::AttackDelay()
{
	AttackReset = true;
	SetSpeed();
	/*if (GetState() == EGhoulState::AttackCooldown)
		SetState(EGhoulState::Chase);*/
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

	if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
	{
		GetMesh()->GetAnimInstance()->StopAllMontages(false);
	}

	if (AIController->GetFocusActor())
		AIController->ClearFocus(EAIFocusPriority::Gameplay);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	/*GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);*/

	SetLifeSpan(10);
}

void AGhoul::AttackCooldown()
{
	if (AttackTarget)
	{
		if (GetType() == EGhoulType::Ranged)
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
			else if (!GetHasLineOfSight(Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))))
			{
				SetSpeed();
				if (TestPathExists(AttackTarget))
					AIController->MoveToActor(AttackTarget, MeleeTargetDistance);
			}
		}
		else if (GetType() == EGhoulType::Melee)
		{
			TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
			ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Camera));
			UClass* SeekClass = ABaseEnemy::StaticClass();
			TArray<AActor*> ActorsToIgnore = { this };
			TArray<AActor*> OutActors;
			UKismetSystemLibrary::SphereOverlapActors(GetWorld(), GetActorLocation(), MeleeSpreadRange, ObjectTypes, SeekClass, ActorsToIgnore, OutActors);

			if (!OutActors.IsEmpty())
			{
				FVector TargetDirection;
				for (int i = 0; i < OutActors.Num(); i++)
					TargetDirection += (GetActorLocation() - OutActors[i]->GetActorLocation()).GetSafeNormal() * MeleeSpreadRange / FVector::Distance(OutActors[i]->GetActorLocation(), GetActorLocation());
				if (FVector::Distance(AttackTarget->GetActorLocation(), GetActorLocation()) < MeleeSpreadRange)
					TargetDirection += (GetActorLocation() - AttackTarget->GetActorLocation()).GetSafeNormal() * MeleeSpreadRange / (FVector::Distance(AttackTarget->GetActorLocation(), GetActorLocation()) * 2);

				FVector TargetPosition;
				if (TargetDirection.Length() > MeleeSpreadRange)
					TargetPosition = GetActorLocation() + TargetDirection.GetSafeNormal() * MeleeSpreadRange;
				else
					TargetPosition = GetActorLocation() + TargetDirection;

				if (TestPathExists(TargetPosition))
					AIController->MoveToLocation(TargetPosition, 0);
			}
			else if (FVector::Distance(GetActorLocation(), AttackTarget->GetActorLocation()) > MeleeSpreadRange)
				SetState(EGhoulState::Chase);
		}
	}
}


void AGhoul::CheckPatrolReset()
{
	if ((GhState == EGhoulState::Attack || GhState == EGhoulState::AttackCooldown) && AttackTarget)
	{
		if (!GetHasLineOfSight(Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))) || !TestPathExists(AttackTarget))
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
}

void AGhoul::PatrolReset()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);

	AttackTarget = nullptr;
	AIController->ClearFocus(EAIFocusPriority::Gameplay);
	AttackReset = true;
	bInCombat = false;

	Stats->Health = Stats->GetMaxHealth();
	UpdateHealthBar.Broadcast();

	SetState(EGhoulState::Idle);

	PawnSensing->SetSensingUpdatesEnabled(true);
}

bool AGhoul::GetHasLineOfSight(ACharacter* Target)
{
	FVector StartLocation = HeadLocation->GetComponentLocation() + GetActorForwardVector() * 50;
	//APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	FVector PlayerLocation = Target->GetActorLocation();
	PlayerLocation.Z += Target->GetCapsuleComponent()->GetLocalBounds().BoxExtent.Z;
	FVector EndLocation = StartLocation + (PlayerLocation - StartLocation).GetSafeNormal() * PatrolResetDistance;
	FHitResult OutHit;

	/*DrawDebugSphere(GetWorld(), StartLocation, 10, 16, FColor::Green, false, 1.0f);
	DrawDebugSphere(GetWorld(), PlayerLocation, 10, 16, FColor::Blue, false, 1.0f);
	DrawDebugSphere(GetWorld(), EndLocation, 10, 16, FColor::Red, false, 1.0f);*/

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), Spike, FoundActors);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	Params.AddIgnoredActors(FoundActors);

	GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, ECollisionChannel::ECC_Camera, Params);

	if (Target == Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)))
		return OutHit.GetActor() == Target || OutHit.GetActor() == Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))->EquippedWeapon;
	else
		return OutHit.GetActor() == Target;
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
	const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	const ANavigationData* NavData = NavSys->GetNavDataForProps(GetNavAgentPropertiesRef());
	if (NavData)
	{
		FPathFindingQuery MyAIQuery = FPathFindingQuery(this, *NavData, GetActorLocation(), Target);
		return NavSys->TestPathSync(MyAIQuery);
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
