// Fill out your copyright notice in the Description page of Project Settings.


#include "Gatekeeper.h"

AGatekeeper::AGatekeeper()
{
	RootComponent = GetCapsuleComponent();
	GetCapsuleComponent()->SetRelativeScale3D(FVector(2.5, 2.5, 2.5));
	GetCapsuleComponent()->SetCapsuleHalfHeight(88);
	GetCapsuleComponent()->SetCapsuleRadius(34);

	//Initialize General Stats
	if (GetAttack() <= 0)
		SetAttack(10);
	Stats->Defense = 20;
	Stats->SetMaxHealth(200);
	Stats->Health = Stats->GetMaxHealth();

	//Initialize Boss Stats
	SetMinAttackDelay(1.5f);
	SetMaxAttackDelay(2);
	SetHPThresholdHigh(0.75f);
	SetHPThresholdMed(0.5f);
	SetHPThresholdLow(0.25f);

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

	if (!Weapon)
		Weapon = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("WeaponMesh")));
	if (!WeaponCollider)
		WeaponCollider = Cast<UBoxComponent>(GetDefaultSubobjectByName(TEXT("WeaponBox")));

	if (WeaponCollider)
		WeaponCollider->OnComponentBeginOverlap.AddDynamic(this, &AGatekeeper::OnComponentBeginOverlap);

	SetState(GatekeeperState::Start);
}

void AGatekeeper::Tick(float DeltaTime)
{
	//if (Attacking)
		//TrackPlayer();

	if (GKState != GatekeeperState::Start && GKState != GatekeeperState::Dead && GKState != GatekeeperState::Staggered)
	{
		if (FVector::Distance(AttackTarget->GetActorLocation(), GetActorLocation()) > ReachTargetDistance && !AIController->IsFollowingAPath())
		{
			if (!GetWorldTimerManager().IsTimerActive(TimerHandle) && !GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
			{
				AIController->MoveToActor(AttackTarget, ReachTargetDistance);
				RandomizeTimeToRun();
			}
		}
	}

	if (AIController->IsFollowingAPath() && GetCharacterMovement()->MaxWalkSpeed == GetWalkSpeed() && GKState != GatekeeperState::Start)
	{
		RunTimer += DeltaTime;
		if (RunTimer >= RandomTimeToRun)
		{
			GetCharacterMovement()->MaxWalkSpeed = GetRunSpeed();
			RunTimer = 0;
		}
	}
}

void AGatekeeper::OnSeePawn(APawn* OtherPawn)
{
	AttackTarget = OtherPawn;
	bInCombat = true;
	AIController->SeePlayer(AttackTarget);

	SetSpeed();

	SetState(GatekeeperState::Chase);

	PawnSensing->SetSensingUpdatesEnabled(false);
}

void AGatekeeper::SetCanWeaponApplyDamage(bool ApplyDamage)
{
	bCanWeaponApplyDamage = ApplyDamage;
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
		RandomizeTimeToRun();
		break;
	case GatekeeperState::HeavyAttack:
		AIController->MoveToActor(AttackTarget, ReachTargetDistance);
		RandomizeTimeToRun();
		break;
	case GatekeeperState::SummonPortals:
		Attacking = false;
		if (PortalReset)
		{
			PortalReset = false;
			if (GetMesh()->GetAnimInstance()->IsAnyMontagePlaying())
				GetMesh()->GetAnimInstance()->StopAllMontages(false);
			GetMesh()->GetAnimInstance()->Montage_Play(SummonPortalMontage);
			GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, SummonPortalMontage);

			StopMovement();
		}
		break;
	case GatekeeperState::Staggered:

		break;
	case GatekeeperState::Dead:
		LockState = true;
		break;
	}
}

void AGatekeeper::StopMovement()
{
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->MaxWalkSpeed = 0;
	Attacking = true;
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
			SetSpeed();
			GetWorldTimerManager().SetTimer(TimerHandle,
				this,
				&AGatekeeper::AttackDelay,
				FMath::RandRange(GetMinAttackDelay(), GetMaxAttackDelay()));
		}
		else if (Montage == SummonPortalMontage)
		{
			SetSpeed();
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

void AGatekeeper::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OverlappedComponent == WeaponCollider && bCanWeaponApplyDamage)
	{
		if (OtherActor == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		{
			float Multiplier = 0;
			UAnimMontage* CurrentMontage = GetMesh()->GetAnimInstance()->GetCurrentActiveMontage();

			if (MontageMap.Contains(CurrentMontage))
			{
				Multiplier = *MontageMap.Find(CurrentMontage);
			}

			UGameplayStatics::ApplyDamage(OtherActor, Attack * Multiplier, NULL, this, NULL);
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
	if (Stats->Health <= Stats->GetMaxHealth() * GetHPThresholdLow())
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
		SetAttack(GetAttack() * AttackMultiplier);
		EquippedWeapon->SetBaseDamage(GetAttack());
		GetMesh()->GetAnimInstance()->Montage_Play(EnrageMontage);
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndDelegate, EnrageMontage);
	}
}

void AGatekeeper::TrackPlayer()
{
	float zLook = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), AttackTarget->GetActorLocation()).Yaw;
	SetActorRotation(UKismetMathLibrary::RLerp(FRotator(0, GetActorRotation().Yaw, 0), FRotator(0, zLook, 0), 0.02f, true));
	
}

void AGatekeeper::RandomizeTimeToRun()
{
	RandomTimeToRun = FMath::RandRange(TimeToRun - 1.0f, TimeToRun + 1.0f);
}

void AGatekeeper::SpawnPortals(int PortalCount)
{
	FActorSpawnParameters SpawnInfo;
	for (int i = 0; i < PortalCount; i++)
	{
		GetWorld()->SpawnActor<APortalSpawn>(Portal, PortalSpawns[i]->GetActorLocation(), FRotator(PortalSpawns[i]->GetActorRotation()), SpawnInfo);
	}
}

void AGatekeeper::Death()
{
	//Set HP Widget Component Not Visible

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->MaxWalkSpeed = 0;
	Attacking = false;
	bIsDead = true;
	bInCombat = false;
	SetState(GatekeeperState::Dead);

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

	Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()))->KillGatekeeperSpawns();

	SetLifeSpan(10);
}

void AGatekeeper::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EquippedWeapon != nullptr)
		EquippedWeapon->Destroy();
	Super::EndPlay(EndPlayReason);

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
	return Stats->Health > Stats->GetMaxHealth() * HPThreshold && Stats->Health - Damage <= Stats->GetMaxHealth() * HPThreshold;
}

void AGatekeeper::UpdateHealth(bool StopMovement, float Damage)
{
	Stats->Health = (Stats->Health - Damage);

	if (StopMovement)
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->MaxWalkSpeed = 0;
	}
}

void AGatekeeper::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	float PortalCount = HealthCheck(FMath::Floor(Damage * (25 / (25 + Stats->Defense * DefenseMultiplier))));

	UpdateHealthBar.Broadcast();

	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, FString::FromInt((int)PortalCount));

	if (PortalCount > 0)
	{
		SpawnPortals(PortalCount);
		SetState(GatekeeperState::SummonPortals);
	}
	else
	{
		if (Stats->Health <= 0)
			Death();
	}
}

void AGatekeeper::AttackTrace(UAnimMontage* AnimTrigger)
{
	Super::AttackTrace(AnimTrigger);

	FVector StartLocation;
	float AttackRadius;
	float Multiplier = 0;

	if (MontageMap.Contains(AnimTrigger))
	{
		Multiplier = *MontageMap.Find(AnimTrigger);
	}

	if (AnimTrigger == StompMontage)
	{
		StartLocation = GetMesh()->GetComponentLocation();
		AttackRadius = StompRadius;
	}
	else
	{
		return;
	}

	TArray<AActor*> ActorsToIgnore = { GetOwner() };
	TArray<AActor*> OutActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Camera));

	UKismetSystemLibrary::SphereOverlapActors(GetWorld(), StartLocation, AttackRadius, ObjectTypes, nullptr, ActorsToIgnore, OutActors);
	if (OutActors.Num() > 0)
	{
		float Damage = FMath::Floor(GetAttack() * FMath::RandRange(0.9f, 1.1f));
		for (int i = 0; i < OutActors.Num(); i++)
		{
			if (OutActors[i] == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
			{
				//DrawDebugLine(GetWorld(), StartLocation, OutActors[i]->GetActorLocation(), FColor::Blue, false, 5);
				UGameplayStatics::ApplyDamage(OutActors[i], Damage * Multiplier, NULL, this, NULL);
				FVector ImpulseVector = OutActors[i]->GetActorLocation() - StartLocation;
				ImpulseVector.Z = 0;
				ImpulseVector = ImpulseVector.GetSafeNormal() * StompImpulseForce;
				Cast<ACharacter>(OutActors[i])->LaunchCharacter(ImpulseVector, true, false);
				return;
			}

		}
	}
}

GatekeeperState AGatekeeper::GetState()
{
	return GKState;
}

void AGatekeeper::SetState(GatekeeperState state)
{
	if (!LockState)
	{
		GKState = state;
		BehaviourStateEvent();
	}
}

