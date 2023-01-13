// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#pragma region Constructor and Inheritied Functions

APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	#pragma region Setup Character Movement

	fGamepadTurnRate = 50.0f;

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

	#pragma endregion

	#pragma region Stats Initializing

	Stats->SetBaseHealth(30.f);
	Stats->GetBaseHealth();

	Stats->SetBaseFocus(20.f);
	Stats->GetBaseFocus();

	Stats->SetBaseStamina(50.f);
	Stats->GetBaseStamina();

	#pragma endregion

	#pragma region Telekinesis Positioning
	
	TelekinesisSource = CreateDefaultSubobject<USceneComponent>(TEXT("Telekinesis Source"));
	TelekinesisSource->SetupAttachment(GetMesh());
	TelekinesisSource->SetRelativeLocation(FVector(-190, 40, 147));
	
	#pragma endregion
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Jumping
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &APlayerCharacter::StopJumping);

	// Running
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &APlayerCharacter::RunStart);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &APlayerCharacter::RunStop);

	// Turning and moving camera
	PlayerInputComponent->BindAxis("Turn / Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn / Gamepad", this, &APlayerCharacter::TurnRate);
	PlayerInputComponent->BindAxis("Look up/down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look up/down Gamepad", this, &APlayerCharacter::TurnRate);

	// Moving the character
	PlayerInputComponent->BindAxis("Move Forward/Backward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right/Left", this, &APlayerCharacter::MoveRight);

	// Attacking
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &APlayerCharacter::Attack);
	PlayerInputComponent->BindAction("AlternateAttack", IE_Pressed, this, &APlayerCharacter::AlternateAttack);

	// Telekinesis
	PlayerInputComponent->BindAction("Telekinesis", IE_Pressed, this, &APlayerCharacter::HandleTelekinesis);

	// Dodging
	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &APlayerCharacter::Dodge);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Checks Player levels to initialize stats
	Stats->InitializeMaxStats();
	Stats->InitializeMainStats();

	EquipFromInventory(0, "LeftWeaponSocket");

	#pragma region Timeline Bindings

	#pragma region Camera Zooming Bindings

	FOnTimelineFloat ZoomUpdate;
	ZoomUpdate.BindUFunction(this, FName("ZoomUpdate"));

	FOnTimelineEvent ZoomFinishedEvent;
	ZoomFinishedEvent.BindUFunction(this, FName("ZoomFinished"));

	ZoomTimeline.AddInterpFloat(ZoomCurve, ZoomUpdate);
	ZoomTimeline.SetTimelineFinishedFunc(ZoomFinishedEvent);

	#pragma endregion

	#pragma region Dodging Bindings

	FOnTimelineFloat DodgingUpdate;
	DodgingUpdate.BindUFunction(this, FName("DodgingUpdate"));	
	DodgingTimer.AddInterpFloat(DodgingCurve, DodgingUpdate);

	#pragma endregion

	#pragma endregion
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ZoomTimeline.TickTimeline(DeltaTime);
	if (bTelekinesis)
		APlayerCharacter::DetectTelekineticObject();

	if (bIsDodging)
		DodgingTimer.TickTimeline(DeltaTime);

	DetermineMovementState();
}

#pragma endregion

#pragma region Weapon and Inventories

void APlayerCharacter::EquipFromInventory(int32 Index, FName EquippingSocket = "LeftWeaponSocket") {
	if (WeaponInventory.IsValidIndex(Index)) {
		FString FilePath = UKismetSystemLibrary::GetPathName(WeaponInventory[Index]);
		UObject* SpawnedActor = Cast<UObject>(StaticLoadObject(UObject::StaticClass(), NULL, *FilePath));
		
		UBlueprint* GeneratedBP = Cast<UBlueprint>(SpawnedActor);
		if (!GeneratedBP) return;

		UClass* SpawnedClass = SpawnedActor->StaticClass();
		if (!SpawnedClass) return;

		if (!GetMesh()->DoesSocketExist(EquippingSocket)) return;

		EquippedWeapon = GetWorld()->SpawnActor<ABaseWeapon>(GeneratedBP->GeneratedClass);
		FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
		EquippedWeapon->AttachToComponent(GetMesh(), TransformRules, EquippingSocket);

		IBaseWeaponInterface* SpawnedWeaponInterface = Cast<IBaseWeaponInterface>(EquippedWeapon);
		if (SpawnedWeaponInterface)
			SpawnedWeaponInterface->Execute_Equip(EquippedWeapon, this);
	}
}

void APlayerCharacter::SwapWeapons() {

}

#pragma endregion

#pragma region Camera

void APlayerCharacter::TurnRate(float Rate)
{
	AddControllerYawInput(Rate * fGamepadTurnRate * GetWorld()->GetDeltaSeconds());

	// Changing the Camera if Telekinesis is enabled
	if (bTelekinesis) AddActorWorldRotation(FRotator(0, 0, Rate * fInputYawScale));
}

void APlayerCharacter::LookUpRate(float Rate)
{
	AddControllerPitchInput(Rate * fGamepadTurnRate * GetWorld()->GetDeltaSeconds());
}

#pragma region Telekinetic Camera Functions
void APlayerCharacter::SetCameraOffset()
{
	FVector RightVector = UKismetMathLibrary::GetRightVector(Controller->GetControlRotation());

	CameraArm->TargetOffset = UKismetMathLibrary::Add_VectorVector(RightVector, CameraOffset);
}

void APlayerCharacter::ZoomUpdate(float Alpha) {
	FVector NewLocation = FMath::Lerp(FVector(0, 0, 0), CameraOffset, Alpha);
	FollowCamera->SetRelativeLocation(NewLocation);
	
	int32 NewArmLength = FMath::Lerp(300, 125, Alpha);
	CameraArm->TargetArmLength = NewArmLength;
}

void APlayerCharacter::ZoomFinished() {

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
	CameraArm->CameraLagSpeed = 10;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	ZoomTimeline.Play();
}

void APlayerCharacter::TelekineticEnd() {
	bTelekinesis = false;
	CameraArm->CameraLagSpeed = 5;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	ZoomTimeline.Reverse();

	ITelekinesisInterface* Interface = Cast<ITelekinesisInterface>(HighlightedReference);
	if (Interface) {
		Interface->Execute_Highlight(HighlightedReference, false);
		HighlightedReference = nullptr;
	}

	ITelekinesisInterface* InterfaceProp = Cast<ITelekinesisInterface>(TelekineticPropReference);
	if (InterfaceProp) {
		InterfaceProp->Execute_Drop(TelekineticPropReference);
		TelekineticPropReference = nullptr;

		// Stop Depleting Focus
		GetWorldTimerManager().ClearTimer(FocusDepletionTimer);
	}
}

void APlayerCharacter::SetTelekineticAttackState(ETelekinesisAttackState state) {
	ETelekineticAttackState = state;
}

void APlayerCharacter::DetectTelekineticObject() {
	/// Tracing for Telekinetic Objects
	if (!TelekineticPropReference) {
		FVector StartTrace = FollowCamera->GetComponentLocation() + UKismetMathLibrary::Multiply_VectorFloat(FollowCamera->GetForwardVector(), fDetectionRadius);
		FVector EndTrace = FollowCamera->GetComponentLocation() + UKismetMathLibrary::Multiply_VectorFloat(FollowCamera->GetForwardVector(), fTelekineticRange);

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes{
			EObjectTypeQuery::ObjectTypeQuery1,		// World Static
			// EObjectTypeQuery::ObjectTypeQuery2,		// World Dynamic
			EObjectTypeQuery::ObjectTypeQuery3,		// Pawn
			EObjectTypeQuery::ObjectTypeQuery4,		// Physics Body
			EObjectTypeQuery::ObjectTypeQuery5,		// Vehicle
			EObjectTypeQuery::ObjectTypeQuery6,		// Destructable
			EObjectTypeQuery::ObjectTypeQuery7		// Telekinetic
		};

		FHitResult Hit;

		bool DidFindObject = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), StartTrace, EndTrace, fDetectionRadius, ObjectTypes, false, { this, EquippedWeapon }, EDrawDebugTrace::ForOneFrame, Hit, true);

		if (!HighlightedReference && DidFindObject) {
			// Find if the hit object has the desired interface
			ITelekinesisInterface* Interface = Cast<ITelekinesisInterface>(Hit.GetActor());
			if (Interface) {
				HighlightedReference = Hit.GetActor();
				Interface->Execute_Highlight(Hit.GetActor(), true);
			}
		}
		else if (HighlightedReference != Hit.GetActor()) {
			ITelekinesisInterface* Interface = Cast<ITelekinesisInterface>(HighlightedReference);
			if (Interface) {
				Interface->Execute_Highlight(HighlightedReference, false);
				TelekineticPropReference = nullptr;
				HighlightedReference = nullptr;
			}
		}
	}
}

#pragma endregion

#pragma region Character Movement

void APlayerCharacter::MoveForward(float Axis) {
	if (!bIsDodging) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		if (bIsRunning)
			GetCharacterMovement()->MaxWalkSpeed = fRunSpeed;
		else
			GetCharacterMovement()->MaxWalkSpeed = fWalkSpeed;

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Axis);
	}
}

void APlayerCharacter::MoveRight(float Axis) {
	if (!bIsDodging) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		if (bIsRunning)
			GetCharacterMovement()->MaxWalkSpeed = fRunSpeed;
		else
			GetCharacterMovement()->MaxWalkSpeed = fWalkSpeed;

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
	else if (!bIsRunning && (PlayerVelocityLength >= 0 && PlayerVelocityLength <= fWalkSpeed))
		EMovementState = EMovementState::EMS_Walking;
	else if (bIsRunning || (PlayerVelocityLength >= 0 && PlayerVelocityLength >= fWalkSpeed && PlayerVelocityLength <= fRunSpeed))
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
	GetWorldTimerManager().SetTimer(StaminaRegenerationTimer, this, &APlayerCharacter::RegenerateStamina, 0.1f, true, fStaminaDelay);
}

#pragma region Dodging

void APlayerCharacter::Dodge() {
	if (EMovementState != EMovementState::EMS_Dodging && DodgeAnimation && Stats->Stamina >= fDodgeStaminaCost) {
		GetMesh()->PlayAnimation(DodgeAnimation, false);
		Stats->Stamina -= fDodgeStaminaCost;
	}
}

void APlayerCharacter::DodgingStarted() {
	EMovementState = EMovementState::EMS_Dodging;
	SetDodging(true);

	if (bTelekinesis) {
		DodgingDirection = UKismetMathLibrary::GetDirectionUnitVector(GetActorForwardVector(), GetVelocity());
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	else DodgingDirection = GetActorForwardVector();

	GetCharacterMovement()->MaxWalkSpeed = fRunSpeed;

	DodgingTimer.PlayFromStart();
	GetWorldTimerManager().ClearTimer(StaminaRegenerationTimer);
}

void APlayerCharacter::DodgingUpdate(const float Alpha) {
	AddMovementInput(DodgingDirection * fRunSpeed, Alpha * 2.25f);
}

void APlayerCharacter::DodgingFinished() {
	EMovementState = EMovementState::EMS_Idle;
	SetDodging(false);

	if (bTelekinesis) GetCharacterMovement()->bOrientRotationToMovement = false;
	else GetCharacterMovement()->bOrientRotationToMovement = true;

	DodgingDirection = FVector::Zero();
	GetCharacterMovement()->MaxWalkSpeed = fWalkSpeed;
	GetWorldTimerManager().SetTimer(StaminaRegenerationTimer, this, &APlayerCharacter::RegenerateStamina, 0.1f, true, fStaminaDelay);
}

#pragma endregion

#pragma endregion

#pragma region Combat

void APlayerCharacter::Attack() {
	if (!bIsDodging) {
		if (!bTelekinesis && EquippedWeapon && (Stats->Stamina >= EquippedWeapon->GetStaminaCost())) {
			Stats->Stamina -= EquippedWeapon->GetStaminaCost();
			EquippedWeapon->Attack();
		}
		else if (bTelekinesis && (TelekineticPropReference || HighlightedReference)) {
			if (Stats->FocusPoints >= fPullFocusCost && ETelekineticAttackState == ETelekinesisAttackState::ETA_None) {
				ITelekinesisInterface* InterfaceFromProp = Cast<ITelekinesisInterface>(HighlightedReference);
				if (InterfaceFromProp) {
					// Setting the Telekinesis Prop Reference and Pulling it
					ETelekineticAttackState = ETelekinesisAttackState::ETA_Pull;
					TelekineticPropReference = HighlightedReference;

					InterfaceFromProp->Execute_Pull(HighlightedReference, this);

					// Start Depleting Focus
					Stats->FocusPoints -= fPullFocusCost;
					if (!GetWorldTimerManager().IsTimerActive(FocusDepletionTimer))
						GetWorldTimerManager().SetTimer(FocusDepletionTimer, this, &APlayerCharacter::DepleteFocus, 0.25f, true);
				}
			}
			else if (Stats->FocusPoints >= fPushFocusCost && ETelekineticAttackState == ETelekinesisAttackState::ETA_Pull || ETelekineticAttackState == ETelekinesisAttackState::ETA_Hold) {
				FVector EndTrace = UKismetMathLibrary::Add_VectorVector(FollowCamera->GetComponentLocation(), UKismetMathLibrary::Multiply_VectorFloat(FollowCamera->GetForwardVector(), fTelekineticRange));

				ITelekinesisInterface* Interface = Cast<ITelekinesisInterface>(TelekineticPropReference);
				if (Interface) {
					ETelekineticAttackState = ETelekinesisAttackState::ETA_None;

					Interface->Execute_Push(TelekineticPropReference, EndTrace, fPushForce);
					TelekineticPropReference = nullptr;

					// Stop Depleting Focus
					Stats->FocusPoints -= fPushFocusCost;
					GetWorldTimerManager().ClearTimer(FocusDepletionTimer);
				}
			}
		}
	}
}

void APlayerCharacter::AlternateAttack() {
	if (!bTelekinesis && EquippedWeapon) 
		EquippedWeapon->UniqueAttack();
	// Telekinetic Dropping
	else if (bTelekinesis && TelekineticPropReference && (ETelekineticAttackState == ETelekinesisAttackState::ETA_Pull || ETelekineticAttackState == ETelekinesisAttackState::ETA_Hold)) {
		ITelekinesisInterface* Interface = Cast<ITelekinesisInterface>(TelekineticPropReference);
		if (Interface) {

			Interface->Execute_Drop(TelekineticPropReference);
			TelekineticPropReference = nullptr;

			// Stop Depleting Focus
			GetWorldTimerManager().ClearTimer(FocusDepletionTimer);
		}
	}
}
#pragma endregion

#pragma region Stat Manipulation

void APlayerCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) {
	if (!bInvincible || !bIsDodging)
	{
		ABaseEntity::TakeAnyDamage(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);
		Stats->Health -= Damage;
		bInvincible = true;
		GetWorld()->GetTimerManager().SetTimer(InvincibilityTimer, this, &APlayerCharacter::ResetInvincibility, 1.5f, false);
		if (!GetWorldTimerManager().IsTimerActive(HealthRegenerationTimer))
			GetWorldTimerManager().ClearTimer(HealthRegenerationTimer);
		GetWorldTimerManager().SetTimer(HealthRegenerationTimer, this, &APlayerCharacter::RegenerateHealth, fHealingDelay, true);
	}
}

void APlayerCharacter::DepleteFocus() {
	Stats->FocusPoints -= fConstantFocusRate * 0.25f;
	if (Stats->FocusPoints <= 0) {
		Stats->FocusPoints = 0;
		GetWorldTimerManager().ClearTimer(FocusDepletionTimer);

		ETelekineticAttackState = ETelekinesisAttackState::ETA_None;

		if (TelekineticPropReference) {
			ITelekinesisInterface* Interface = Cast<ITelekinesisInterface>(TelekineticPropReference);
			if (Interface) {
				Interface->Execute_Drop(TelekineticPropReference);
				TelekineticPropReference = nullptr;
			}
		}
	}
}

void APlayerCharacter::DepleteStamina() {
	if (EMovementState == EMovementState::EMS_Running) {
		Stats->Stamina -= fRunningStaminaDepletionRate * 0.1f;

		if (Stats->Stamina <= 0) {
			Stats->Stamina = 0;
			bIsRunning = false;
			GetWorldTimerManager().ClearTimer(StaminaRegenerationTimer);
			GetWorldTimerManager().SetTimer(StaminaRegenerationTimer, this, &APlayerCharacter::RegenerateStamina, 0.1f, true, fStaminaDelay);
		}
	}
}

void APlayerCharacter::RegenerateHealth() {
	Stats->Health += fHealingRate;

	if (Stats->Health > Stats->GetMaxHealth()) {
		Stats->Health = Stats->GetMaxHealth();
		GetWorldTimerManager().ClearTimer(HealthRegenerationTimer);
	}
}

void APlayerCharacter::RegenerateStamina() {
	Stats->Stamina += fStaminaRate * 0.1f;

	if (Stats->Stamina > Stats->GetMaxStamina()) {
		Stats->Stamina = Stats->GetMaxStamina();
		GetWorldTimerManager().ClearTimer(StaminaRegenerationTimer);
	}
}

#pragma endregion