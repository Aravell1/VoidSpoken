// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "FocusPickup.h"
#include "HealthPickup.h"
#include "StaminaPickup.h"
#include "CombatDirector.h"
#include "Camera/PlayerCameraManager.h"

// Need these weapons to show up as active!!! REMEMVER!@!

#pragma region Constructor and Inheritied Functions

APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	#pragma region Setup Character Movement

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = 700.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	static ConstructorHelpers::FObjectFinder<UAnimMontage>C_HitMontage(TEXT("/Game/Blueprints/Player/Animations/Sequences/Montages/Anim_Hit_Montage.Anim_Hit_Montage"));
	if (C_HitMontage.Succeeded())
		HitMontage = C_HitMontage.Object;

	#pragma region Dodging

	static ConstructorHelpers::FObjectFinder<UCurveFloat>C_DodgeCurve(TEXT("/Game/Blueprints/Player/Animations/Sequences/Dodging/Dodge.Dodge"));
	if (C_DodgeCurve.Succeeded())
		DodgingCurve = C_DodgeCurve.Object;
	
	//static ConstructorHelpers::FObjectFinder<UAnimMontage>C_DodgingAnimation(TEXT("/Game/Blueprints/Player/Animations/Sequences/Montages/Dodging_Montage.Dodging_Montage"));
	static ConstructorHelpers::FObjectFinder<UAnimMontage>C_DodgingAnimation(TEXT("/Game/Blueprints/Player/Animations/Sequences/Montages/Anim_AttemptDodge_Montage.Anim_AttemptDodge_Montage"));
	if (C_DodgingAnimation.Succeeded())
		DodgeAnimation = C_DodgingAnimation.Object;

	#pragma endregion

	#pragma endregion

	#pragma region Setup Camera

	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera arm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->TargetArmLength = 300;
	CameraArm->bUsePawnControlRotation = true;
	CameraArm->bEnableCameraLag = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// CurveFloat'/Game/Blueprints/Player/Telekinesis/Zoom.Zoom'

	static ConstructorHelpers::FObjectFinder<UCurveFloat>C_ZoomCurve(TEXT("/Game/Blueprints/Player/Telekinesis/Zoom.Zoom"));
	if (C_ZoomCurve.Succeeded())
		ZoomCurve = C_ZoomCurve.Object;

	#pragma endregion

	#pragma region Stats Initializing

	Stats->SetBaseHealth(50.f);
	Stats->GetBaseHealth();

	Stats->SetBaseFocus(50.f);
	Stats->GetBaseFocus();

	Stats->SetBaseStamina(50.f);
	Stats->GetBaseStamina();

	HealAmount = 10.0f;
	FocusAmount = 25.0f;
	StaminaAmount = 12.5f;

	#pragma endregion

	#pragma region Telekinesis Positioning
	
	TelekinesisSource = CreateDefaultSubobject<USceneComponent>(TEXT("Telekinesis Source"));
	TelekinesisSource->SetupAttachment(GetMesh());
	TelekinesisSource->SetRelativeLocation(FVector(-175, 30, 125));
	
	#pragma endregion
	
	bIsFDown = false;
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Running
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &APlayerCharacter::RunStart);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &APlayerCharacter::RunStop);

	// Interactions / Items
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &APlayerCharacter::On_F_Down);
	PlayerInputComponent->BindAction("Interact", IE_Released, this, &APlayerCharacter::On_F_Release);

	// Turning and moving camera
	PlayerInputComponent->BindAxis("Turn / Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn / Gamepad", this, &APlayerCharacter::TurnRate);
	PlayerInputComponent->BindAxis("Look up/down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look up/down Gamepad", this, &APlayerCharacter::LookUpRate);

	// Moving the character
	PlayerInputComponent->BindAxis("Move Forward/Backward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right/Left", this, &APlayerCharacter::MoveRight);

	// Attacking
	PlayerInputComponent->BindAction("LeftAttack", IE_Pressed, this, &APlayerCharacter::LeftAttack);
	PlayerInputComponent->BindAction("RightAttack", IE_Pressed, this, &APlayerCharacter::RightAttack);

	// Telekinesis
	PlayerInputComponent->BindAction("Telekinesis", IE_Pressed, this, &APlayerCharacter::HandleTelekinesis);

	// Dodging
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &APlayerCharacter::Dodge);

	//Use Item
	//PlayerInputComponent->BindAction("Use", IE_Pressed, this, &APlayerCharacter::);
}

void APlayerCharacter::BeginPlay() {
	Super::BeginPlay();
	
	// Checks Player levels to initialize stats
	Stats->InitializeMaxStats();
	Stats->InitializeMainStats();

	EquipFromInventory(0, "Left");
	EquipFromInventory(1, "Right");
	DodgingMaterialInterface = GetMesh()->GetMaterial(0);

	#pragma region Combat Director Declaration

		TArray<AActor*> FoundDirectors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACombatDirector::StaticClass(), FoundDirectors);
		if (Cast<ACombatDirector>(FoundDirectors[0]))
		{
			CombatDirector = Cast<ACombatDirector>(FoundDirectors[0]);
		}

	#pragma endregion 

	#pragma region Timeline Bindings

	#pragma region Camera Zooming Bindings

	FOnTimelineFloat ZoomUpdate;
	ZoomUpdate.BindUFunction(this, FName("ZoomUpdate"));

	ZoomTimeline.AddInterpFloat(ZoomCurve, ZoomUpdate);

	#pragma endregion

	#pragma region Dodging Bindings

	FOnTimelineFloat DodgingUpdate;
	DodgingUpdate.BindUFunction(this, FName("DodgingUpdate"));	
	DodgingTimer.AddInterpFloat(DodgingCurve, DodgingUpdate);

	#pragma endregion

	#pragma endregion
}

void APlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	DetermineMovementState();
	//AddMovementInput(bTelekinesis ? GetVelocity() : GetActorForwardVector(), GetMesh()->GetAnimInstance()->GetCurveValue(FName("Movement Delta (Forward)")));
	AddMovementInput(GetVelocity(), GetMesh()->GetAnimInstance()->GetCurveValue(FName("Movement Delta (Forward)")));
	
	if (CombatDirector && bInCombat != CombatDirector->GetInCombat() && !GetWorldTimerManager().IsTimerActive(CombatTimer)) {
		GetWorldTimerManager().ClearTimer(CombatTimer);
		bInCombat = CombatDirector->GetInCombat();

		if (bInCombat) EquippedWeapon->Show();
	}

	ZoomTimeline.TickTimeline(DeltaTime);
	if (bTelekinesis)
		DetectTelekineticObject();

	if (bIsDodging)
		DodgingTimer.TickTimeline(DeltaTime);
}

#pragma endregion

#pragma region Weapon and Inventories

void APlayerCharacter::EquipFromInventory(int32 Index, FName EquippingSlot = FName("Left")) {
	if (WeaponInventory.IsValidIndex(Index)) {
		if (!GetMesh()->DoesSocketExist("LeftWeaponSocket")) return;

		ABaseWeapon* ActiveWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponInventory[Index]);
		const FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
		ActiveWeapon->AttachToComponent(GetMesh(), TransformRules, "LeftWeaponSocket");

		if (EquippingSlot == "Right") RightEquippedWeapon = ActiveWeapon;
		else LeftEquippedWeapon = ActiveWeapon;
		EquippedWeapon = ActiveWeapon;
		EquippedWeapon->HideCompletely();
		
		if (const IBaseWeaponInterface* SpawnedWeaponInterface = Cast<IBaseWeaponInterface>(EquippedWeapon)) {
			SpawnedWeaponInterface->Execute_Equip(EquippedWeapon, this, EquippingSlot);
			EquippedWeapon->OnAttackStarted.BindUObject(this, &APlayerCharacter::OnWeaponAttackStarted);
			EquippedWeapon->OnAttackEnded.BindUObject(this, &APlayerCharacter::OnWeaponAttackEnded);
		}
	}
}

void APlayerCharacter::ResetCameraRotation() {
	GetWorld()->GetFirstPlayerController()->SetControlRotation(GetActorRotation());
}

#pragma endregion

#pragma region Camera

void APlayerCharacter::TurnRate(const float Rate) {
	AddControllerYawInput(Rate * GamepadTurnRate * GetWorld()->GetDeltaSeconds());

	// Changing the Camera if Telekinesis is enabled
	if (bTelekinesis) AddActorWorldRotation(FRotator(0, 0, Rate * InputYawScale));
}

void APlayerCharacter::LookUpRate(const float Rate) {
	AddControllerPitchInput(Rate * GamepadTurnRate * GetWorld()->GetDeltaSeconds());
}

#pragma region Telekinetic Camera Functions

void APlayerCharacter::SetCameraOffset() const {
	const FVector RightVector = UKismetMathLibrary::GetRightVector(Controller->GetControlRotation());
	CameraArm->TargetOffset = UKismetMathLibrary::Add_VectorVector(RightVector, CameraOffset);
}

void APlayerCharacter::ZoomUpdate(const float Alpha) const {
	const FVector NewLocation = FMath::Lerp(FVector::ZeroVector, CameraOffset, Alpha);
	FollowCamera->SetRelativeLocation(NewLocation);
	
	const int32 NewArmLength = FMath::Lerp(300, 125, Alpha);
	CameraArm->TargetArmLength = NewArmLength;
}

#pragma endregion

#pragma endregion

#pragma region Telekinetic Functionality

void APlayerCharacter::HandleTelekinesis() {
	if (!bTelekinesis) TelekineticStart();
	else TelekineticEnd();
}

void APlayerCharacter::TelekineticStart() {
	bTelekinesis = true;
	CameraArm->CameraLagSpeed = 10.0f;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	ZoomTimeline.Play();
}

void APlayerCharacter::TelekineticEnd() {
	bTelekinesis = false;
	CameraArm->CameraLagSpeed = 5.0f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	ZoomTimeline.Reverse();

	if (const ITelekinesisInterface* Interface = Cast<ITelekinesisInterface>(HighlightedReference)) {
		Interface->Execute_Highlight(HighlightedReference, false);
		HighlightedReference = nullptr;
	}

	if (const ITelekinesisInterface* InterfaceProp = Cast<ITelekinesisInterface>(TelekineticPropReference)) {
		InterfaceProp->Execute_Drop(TelekineticPropReference);
		TelekineticPropReference = nullptr;

		// Stop Depleting Focus
		GetWorldTimerManager().ClearTimer(FocusDepletionTimer);
	}
}

void APlayerCharacter::DetectTelekineticObject() {
	/// Tracing for Telekinetic Objects
	if (!TelekineticPropReference) {
		FVector StartTrace = FollowCamera->GetComponentLocation() + UKismetMathLibrary::Multiply_VectorFloat(FollowCamera->GetForwardVector(), DetectionRadius);
		FVector EndTrace = FollowCamera->GetComponentLocation() + UKismetMathLibrary::Multiply_VectorFloat(FollowCamera->GetForwardVector(), TelekineticRange);

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{
			EObjectTypeQuery::ObjectTypeQuery1,		// World Static
			// EObjectTypeQuery::ObjectTypeQuery2,		// World Dynamic
			EObjectTypeQuery::ObjectTypeQuery3,		// Pawn
			EObjectTypeQuery::ObjectTypeQuery4,		// Physics Body
			EObjectTypeQuery::ObjectTypeQuery5,		// Vehicle
			EObjectTypeQuery::ObjectTypeQuery6,		// Destructible
			EObjectTypeQuery::ObjectTypeQuery7		// Telekinetic
		};

		FHitResult Hit;

		const bool DidFindObject = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), StartTrace, EndTrace, DetectionRadius, ObjectTypes, false, { this, LeftEquippedWeapon, RightEquippedWeapon }, EDrawDebugTrace::ForOneFrame, Hit, true);

		if (!HighlightedReference && DidFindObject) {
			// Find if the hit object has the desired interface
			if (ITelekinesisInterface* Interface = Cast<ITelekinesisInterface>(Hit.GetActor())) {
				HighlightedReference = Hit.GetActor();
				Interface->Execute_Highlight(Hit.GetActor(), true);
			}
		}
		else if (HighlightedReference != Hit.GetActor()) {
			if (ITelekinesisInterface* Interface = Cast<ITelekinesisInterface>(HighlightedReference)) {
				Interface->Execute_Highlight(HighlightedReference, false);
				TelekineticPropReference = nullptr;
				HighlightedReference = nullptr;
			}
		}
	}
}

#pragma endregion

#pragma region Character Movement

void APlayerCharacter::MoveForward(const float Axis) {
	PlayerInput.X = Axis;
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	if (!bIsDodging) {
		if (bIsRunning)
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		else
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Axis);
	}
}

void APlayerCharacter::MoveRight(const float Axis) {
	PlayerInput.Y = Axis;
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	if (!bIsDodging) {
		if (bIsRunning)
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		else
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Axis);
	}
}

void APlayerCharacter::DetermineMovementState() {
	const float PlayerVelocityLength = GetCharacterMovement()->Velocity.Size();
	if (bIsDodging)
		EMovementState = EMovementState::EMS_Dodging;
	else if (PlayerVelocityLength <= 0)
		EMovementState = EMovementState::EMS_Idle;
	else if (!bIsRunning && (PlayerVelocityLength >= 0 && PlayerVelocityLength <= WalkSpeed))
		EMovementState = EMovementState::EMS_Walking;
	else if (bIsRunning || (PlayerVelocityLength >= 0 && PlayerVelocityLength >= WalkSpeed && PlayerVelocityLength <= RunSpeed))
		EMovementState = EMovementState::EMS_Running;
}

void APlayerCharacter::RunStart() {
	if (Stats->Stamina > 0.0f && EMovementState != EMovementState::EMS_Idle) {
		bIsRunning = true;
		GetWorldTimerManager().ClearTimer(StaminaRegenerationTimer);
		GetWorldTimerManager().SetTimer(StaminaRegenerationTimer, this, &APlayerCharacter::DepleteStamina, 0.1f, true);
	}
}

void APlayerCharacter::RunStop() {
	bIsRunning = false;
	GetWorldTimerManager().ClearTimer(StaminaRegenerationTimer);
	GetWorldTimerManager().SetTimer(StaminaRegenerationTimer, this, &APlayerCharacter::RegenerateStamina, 0.1f, true, StaminaDelay);
}

#pragma region Dodging

void APlayerCharacter::Dodge() {
	if (!bIsDodging && !LeftEquippedWeapon->bAttackDelay && !RightEquippedWeapon->bAttackDelay && EMovementState != EMovementState::EMS_Dodging && DodgeAnimation && Stats->Stamina >= DodgeStaminaCost) {
		LeftEquippedWeapon->Reset();
		RightEquippedWeapon->Reset();
		bIsAttacking = false;
		Stats->Stamina -= DodgeStaminaCost;
		bIsDodging = true;
		GetMesh()->GetAnimInstance()->Montage_Play(DodgeAnimation);
	}
}

void APlayerCharacter::DodgingStarted()
{
	EMovementState = EMovementState::EMS_Dodging;
	bInvincible = true;
	GetCharacterMovement()->MaxWalkSpeed = 1000.0f;

	if (bTelekinesis)
		GetCharacterMovement()->bOrientRotationToMovement = true;

	PlayerInput.Normalize();
	FVector NewDirection = PlayerInput.X * FollowCamera->GetForwardVector() + PlayerInput.Y * FollowCamera->GetRightVector();
	NewDirection.Normalize();

	if (NewDirection.Size() == 0)
		NewDirection = GetActorForwardVector();
		
	DodgingDirection = NewDirection;

	DodgingTimer.PlayFromStart();
	GetWorldTimerManager().ClearTimer(StaminaRegenerationTimer);
}

void APlayerCharacter::DodgingUpdate(const float Alpha) {
	AddMovementInput(DodgingDirection, 1.0f);
	
	UMaterialInstanceDynamic* DynPlayerMaterial = UMaterialInstanceDynamic::Create(DodgingMaterialInterface, GetMesh());
	DynPlayerMaterial->SetScalarParameterValue("Opaque", Alpha);
	GetMesh()->SetMaterial(0, DynPlayerMaterial);

	if (bInCombat) {
		UMaterialInstanceDynamic* DynWeaponMaterial = UMaterialInstanceDynamic::Create(EquippedWeapon->WeaponMaterialInterface, EquippedWeapon);
		DynWeaponMaterial->SetScalarParameterValue("Opaque", Alpha);
		EquippedWeapon->WeaponMeshComponent->SetMaterial(0, DynWeaponMaterial);
	}
}

void APlayerCharacter::DodgingFinished() {
	bIsDodging = false;
	bInvincible = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	if (bTelekinesis) GetCharacterMovement()->bOrientRotationToMovement = false;
	else GetCharacterMovement()->bOrientRotationToMovement = true;

	DodgingDirection = FVector::Zero();
	DodgingTimer.Stop();

	if (bIsRunning) {
		GetWorldTimerManager().ClearTimer(StaminaRegenerationTimer);
		GetWorldTimerManager().SetTimer(StaminaRegenerationTimer, this, &APlayerCharacter::DepleteStamina, 0.1f, true);
	}
	else GetWorldTimerManager().SetTimer(StaminaRegenerationTimer, this, &APlayerCharacter::RegenerateStamina, 0.1f, true, StaminaDelay);
}

void APlayerCharacter::ResetDodging() {
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
}

#pragma endregion

#pragma endregion

#pragma region Combat

void APlayerCharacter::LeftAttack() {
	if (!bIsDodging) {
		if (!bIsAttacking && !bTelekinesis && LeftEquippedWeapon && Stats->Stamina >= LeftEquippedWeapon->GetStaminaCost() && !LeftEquippedWeapon->GetAttackDelay()) {
			if (RightEquippedWeapon) RightEquippedWeapon->Reset();
			LeftEquippedWeapon->Show();
			RightEquippedWeapon->Hide();
			EquippedWeapon = LeftEquippedWeapon;
			EquippedWeapon->Attack();
		}
		else if (bTelekinesis && (TelekineticPropReference || HighlightedReference))
		{
			if (Stats->FocusPoints >= PullFocusCost && ETelekineticAttackState == ETelekinesisAttackState::ETA_None) {
				if (const ITelekinesisInterface* InterfaceFromProp = Cast<ITelekinesisInterface>(HighlightedReference)) {
					// Setting the Telekinesis Prop Reference and Pulling it
					ETelekineticAttackState = ETelekinesisAttackState::ETA_Pull;
					TelekineticPropReference = HighlightedReference;

					InterfaceFromProp->Execute_Pull(HighlightedReference, this);

					// Start Depleting Focus
					Stats->FocusPoints -= PullFocusCost;
					if (!GetWorldTimerManager().IsTimerActive(FocusDepletionTimer))
						GetWorldTimerManager().SetTimer(FocusDepletionTimer, this, &APlayerCharacter::DepleteFocus, 0.25f, true);
				}
			}
			else if (Stats->FocusPoints >= PushFocusCost && ETelekineticAttackState == ETelekinesisAttackState::ETA_Pull || ETelekineticAttackState == ETelekinesisAttackState::ETA_Hold) {
				const FVector EndTrace = UKismetMathLibrary::Add_VectorVector(FollowCamera->GetComponentLocation(), UKismetMathLibrary::Multiply_VectorFloat(FollowCamera->GetForwardVector(), TelekineticRange));

				if (const ITelekinesisInterface* Interface = Cast<ITelekinesisInterface>(TelekineticPropReference)) {
					ETelekineticAttackState = ETelekinesisAttackState::ETA_None;

					Interface->Execute_Push(TelekineticPropReference, EndTrace, PushForce);
					TelekineticPropReference = nullptr;

					// Stop Depleting Focus
					Stats->FocusPoints -= PushFocusCost;
					GetWorldTimerManager().ClearTimer(FocusDepletionTimer);
				}
			}
		}
	}
}

void APlayerCharacter::RightAttack() {
	if (!bIsDodging) {
		if (!bIsAttacking && !bTelekinesis && RightEquippedWeapon && Stats->Stamina >= RightEquippedWeapon->GetStaminaCost() && !RightEquippedWeapon->GetAttackDelay()) {
			if (LeftEquippedWeapon) LeftEquippedWeapon->Reset();
			RightEquippedWeapon->Show();
			LeftEquippedWeapon->Hide();
			EquippedWeapon = RightEquippedWeapon;
			EquippedWeapon->Attack();
		}
		// Telekinetic Dropping
		else if (bTelekinesis && TelekineticPropReference && (ETelekineticAttackState == ETelekinesisAttackState::ETA_Pull || ETelekineticAttackState == ETelekinesisAttackState::ETA_Hold)) {
			if (const ITelekinesisInterface* Interface = Cast<ITelekinesisInterface>(TelekineticPropReference)) {
				Interface->Execute_Drop(TelekineticPropReference);
				TelekineticPropReference = nullptr;

				// Stop Depleting Focus
				GetWorldTimerManager().ClearTimer(FocusDepletionTimer);
			}
		}
	}
}

#pragma region Attack Delegate Functions

void APlayerCharacter::OnWeaponAttackStarted() {
	bIsAttacking = true;
	GetWorldTimerManager().ClearTimer(CombatTimer);
	GetWorldTimerManager().ClearTimer(StaminaRegenerationTimer);
	
	Stats->Stamina -= EquippedWeapon->GetStaminaCost();
}

void APlayerCharacter::OnWeaponAttackEnded() {
	// Stamina
	GetWorldTimerManager().ClearTimer(StaminaRegenerationTimer);
	GetWorldTimerManager().SetTimer(StaminaRegenerationTimer, this, &APlayerCharacter::RegenerateStamina, 0.1f, true, StaminaDelay);
	
	// Combat
	bIsAttacking = false;
	bInCombat = true;
	GetWorldTimerManager().ClearTimer(CombatTimer);
	if (!CombatDirector->GetInCombat())
		GetWorldTimerManager().SetTimer(CombatTimer, this, &APlayerCharacter::SetCombatState, 1.0f, false, CombatDelay);
}

#pragma endregion

#pragma endregion

#pragma region Stat Manipulation

void APlayerCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) {
	if ((!bInvincible || !bIsDodging)) {
		if (!bIsDead && Stats->Health <= 0) {
			Death();
			bIsDead = true;
		}
		else if (bIsDead) return;
		
		ABaseEntity::TakeAnyDamage(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);
		Stats->Health -= Damage;
		
		bInvincible = true;
		GetWorld()->GetTimerManager().SetTimer(InvincibilityTimer, this, &APlayerCharacter::ResetInvincibility, 0.75f, false);

		if (LeftEquippedWeapon) LeftEquippedWeapon->Reset();
		if (RightEquippedWeapon) RightEquippedWeapon->Reset();
		bIsAttacking = false;
		
		if (!CombatDirector->GetInCombat()) {
			if (!GetWorldTimerManager().IsTimerActive(HealthRegenerationTimer))
				GetWorldTimerManager().ClearTimer(HealthRegenerationTimer);
			GetWorldTimerManager().SetTimer(HealthRegenerationTimer, this, &APlayerCharacter::RegenerateHealth, HealingDelay, true);
		}
	}
}

void APlayerCharacter::OnStaggered() {
	if (LeftEquippedWeapon) LeftEquippedWeapon->Reset();
	if (RightEquippedWeapon) RightEquippedWeapon->Reset();
	GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetMesh()->GetAnimInstance()->Montage_Play(HitMontage);
	GetWorld()->GetTimerManager().SetTimer(InvincibilityTimer, this, &APlayerCharacter::ResetInvincibility, 0.75f, false);

	if (bTelekinesis && TelekineticPropReference && (ETelekineticAttackState == ETelekinesisAttackState::ETA_Pull || ETelekineticAttackState == ETelekinesisAttackState::ETA_Hold)) {
		if (const ITelekinesisInterface* Interface = Cast<ITelekinesisInterface>(TelekineticPropReference)) {
			Interface->Execute_Drop(TelekineticPropReference);
			TelekineticPropReference = nullptr;

			// Stop Depleting Focus
			GetWorldTimerManager().ClearTimer(FocusDepletionTimer);
		}
	}
}

void APlayerCharacter::Death_Implementation() {
	
}


void APlayerCharacter::DepleteFocus() {
	Stats->FocusPoints -= ConstantFocusRate * 0.25f;
	if (Stats->FocusPoints <= 0) {
		Stats->FocusPoints = 0;
		GetWorldTimerManager().ClearTimer(FocusDepletionTimer);

		ETelekineticAttackState = ETelekinesisAttackState::ETA_None;

		if (TelekineticPropReference) {
			if (const ITelekinesisInterface* Interface = Cast<ITelekinesisInterface>(TelekineticPropReference)) {
				Interface->Execute_Drop(TelekineticPropReference);
				TelekineticPropReference = nullptr;
			}
		}
	}
}

void APlayerCharacter::DepleteStamina() {
	if (EMovementState == EMovementState::EMS_Running) {
		Stats->Stamina -= RunningStaminaDepletionRate * 0.1f;

		if (Stats->Stamina <= 0) {
			Stats->Stamina = 0;
			bIsRunning = false;
			GetWorldTimerManager().ClearTimer(StaminaRegenerationTimer);
			GetWorldTimerManager().SetTimer(StaminaRegenerationTimer, this, &APlayerCharacter::RegenerateStamina, 0.1f, true, StaminaDelay);
		}
	}
}

void APlayerCharacter::RegenerateHealth() {
	Stats->Health += HealingRate;

	if (Stats->Health > Stats->GetMaxHealth()) {
		Stats->Health = Stats->GetMaxHealth();
		GetWorldTimerManager().ClearTimer(HealthRegenerationTimer);
	}
}

void APlayerCharacter::RegenerateStamina() {
	Stats->Stamina += StaminaRate * 0.1f;

	if (Stats->Stamina > Stats->GetMaxStamina()) {
		Stats->Stamina = Stats->GetMaxStamina();
		GetWorldTimerManager().ClearTimer(StaminaRegenerationTimer);
	}
}

#pragma endregion

#pragma region Items

void APlayerCharacter::On_F_Down()
{
	bIsFDown = true;

	if (OverlappingItem)
	{
		AFocusPickup* PickupF = Cast<AFocusPickup>(OverlappingItem);
		AHealthPickup* PickupH = Cast<AHealthPickup>(OverlappingItem);
		AStaminaPickup* PickupS = Cast<AStaminaPickup>(OverlappingItem);

		if (PickupF)
		{
			PickupF->PickupFocus();
			SetOverlappingItem(nullptr);
		}
		else if (PickupH)
		{
			PickupH->PickupHealth();
			SetOverlappingItem(nullptr);
		}
		else if (PickupS)
		{
			PickupS->PickupStamina();
			SetOverlappingItem(nullptr);
		}
	}
}

void APlayerCharacter::On_F_Release()
{
	bIsFDown = false;
}


void APlayerCharacter::UseHealthConsumable()
{
	AVoidSpokenGameModeBase* GM;
	GM = Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GM->HealPickup > 0)
	{
		if (Stats->Health < Stats->GetMaxHealth())
		{
			Stats->Health += HealAmount;
			GM->HealPickup -= 1;

			if (Stats->Health >= Stats->GetMaxHealth())
			{
				Stats->Health = Stats->GetMaxHealth();
			}
		}
	}
	else
		return;
}

void APlayerCharacter::UseFocusConsumable()
{
	AVoidSpokenGameModeBase* GM;
	GM = Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GM->FocusPickup > 0)
	{
		if (Stats->FocusPoints < Stats->GetMaxFocus())
		{
			Stats->FocusPoints += FocusAmount;
			GM->FocusPickup -= 1;

			if (Stats->FocusPoints >= Stats->GetMaxFocus())
			{
				Stats->FocusPoints = Stats->GetMaxFocus();
			}
		}
	}
	else
		return;
}

void APlayerCharacter::UseStaminaConsumable()
{
	AVoidSpokenGameModeBase* GM = Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GM && GM->StaminaPickup > 0)
	{
		if (Stats->Stamina < Stats->GetMaxStamina())
		{
			Stats->Stamina += StaminaAmount;
			GM->StaminaPickup -= 1;

			if (Stats->Stamina >= Stats->GetMaxStamina())
			{
				Stats->Stamina = Stats->GetMaxStamina();
			}
		}
	}
}

#pragma endregion
