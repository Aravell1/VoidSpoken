// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

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

	#pragma region Dodging

	DodgingTrailComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Dodge Trail Component"));

	static ConstructorHelpers::FObjectFinder<UCurveFloat>C_DodgeCurve(TEXT("/Game/Blueprints/Player/Animations/Sequences/Dodging/Dodge.Dodge"));
	if (C_DodgeCurve.Succeeded())
		DodgingCurve = C_DodgeCurve.Object;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>C_DodgeTrail(TEXT("/Game/Blueprints/Player/Trails_And_Decals/DashTrail.DashTrail"));
	if (C_DodgeTrail.Succeeded()) {
		DodgingTrailSystem = C_DodgeTrail.Object;
		DodgingTrailComponent->SetAsset(DodgingTrailSystem, true);
		DodgingTrailComponent->SetAutoActivate(false);
	}

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

	Stats->SetBaseHealth(30.f);
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
	TelekinesisSource->SetRelativeLocation(FVector(-250, 30, 200));
	
	#pragma endregion

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Running
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &APlayerCharacter::RunStart);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &APlayerCharacter::RunStop);

	// Turning and moving camera
	PlayerInputComponent->BindAxis("Turn / Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn / Gamepad", this, &APlayerCharacter::TurnRate);
	PlayerInputComponent->BindAxis("Look up/down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look up/down Gamepad", this, &APlayerCharacter::LookUpRate);

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

	//Use Item
	//PlayerInputComponent->BindAction("Use", IE_Pressed, this, &APlayerCharacter::);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Checks Player levels to initialize stats
	Stats->InitializeMaxStats();
	Stats->InitializeMainStats();

	EquipFromInventory(0, "LeftWeaponSocket");
	DodgingMaterialInterface = GetMesh()->GetMaterial(0);
	DodgingTrailComponent->Deactivate();

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

	#pragma region Combat and Attacking Delegates Binding
	
	if (EquippedWeapon) {
		EquippedWeapon->OnAttackStarted.BindUObject(this, &APlayerCharacter::OnWeaponAttackStarted);
		EquippedWeapon->OnAttackEnded.BindUObject(this, &APlayerCharacter::OnWeaponAttackEnded);
	}

	#pragma endregion
}

void APlayerCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	
	/// Add "Relative" Movement endings to the character when in Telekinesis
	if (EMovementState == EMovementState::EMS_Stopping)
		/// Change this to AddMovementInput
		AddActorLocalOffset(FVector(GetMesh()->GetAnimInstance()->GetCurveValue("Movement Delta (Forward)"), 0.0f, 0.0f));
	else DetermineMovementState();

	ZoomTimeline.TickTimeline(DeltaTime);
	if (bTelekinesis)
		APlayerCharacter::DetectTelekineticObject();

	if (bIsDodging)
		DodgingTimer.TickTimeline(DeltaTime);
}

#pragma endregion

#pragma region Weapon and Inventories

void APlayerCharacter::EquipFromInventory(int32 Index, FName EquippingSocket = "LeftWeaponSocket") {
	if (WeaponInventory.IsValidIndex(Index)) {
		if (!GetMesh()->DoesSocketExist(EquippingSocket)) return;

		EquippedWeapon = GetWorld()->SpawnActor<ABaseWeapon>(WeaponInventory[Index]);
		const FAttachmentTransformRules TransformRules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
		EquippedWeapon->AttachToComponent(GetMesh(), TransformRules, EquippingSocket);

		if (const IBaseWeaponInterface* SpawnedWeaponInterface = Cast<IBaseWeaponInterface>(EquippedWeapon)) {
			SpawnedWeaponInterface->Execute_Equip(EquippedWeapon, this);
			EquippedWeapon->OnAttackStarted.BindUObject(this, &APlayerCharacter::OnWeaponAttackStarted);
			EquippedWeapon->OnAttackEnded.BindUObject(this, &APlayerCharacter::OnWeaponAttackEnded);
		}
	}
}

void APlayerCharacter::SwapWeapons() {
	// Swap weapons and rebind delegates

	#pragma region Attacking Delegates Binding

	if (EquippedWeapon) {
		EquippedWeapon->OnAttackStarted.BindUObject(this, &APlayerCharacter::OnWeaponAttackStarted);
		EquippedWeapon->OnAttackEnded.BindUObject(this, &APlayerCharacter::OnWeaponAttackEnded);
	}

	#pragma endregion
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

		const bool DidFindObject = UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), StartTrace, EndTrace, DetectionRadius, ObjectTypes, false, { this, EquippedWeapon }, EDrawDebugTrace::ForOneFrame, Hit, true);

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
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	if (!bIsDodging) {
		if (bIsRunning)
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		else
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	AddMovementInput(Direction, Axis);
}

void APlayerCharacter::MoveRight(const float Axis) {
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	if (!bIsDodging) {
		if (bIsRunning)
			GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		else
			GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(Direction, Axis);
}

void APlayerCharacter::DetermineMovementState() {
	const float PlayerVelocityLength = GetCharacterMovement()->Velocity.Size();
	if (bIsDodging)
		EMovementState = EMovementState::EMS_Dodging;
	else if ((EMovementState == EMovementState::EMS_Running || EMovementState == EMovementState::EMS_Walking) && PlayerVelocityLength == 0.0f)
		EMovementState = EMovementState::EMS_Stopping;
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
	if (!bIsDodging && !EquippedWeapon->bIsAttacking && EMovementState != EMovementState::EMS_Dodging && DodgeAnimation && Stats->Stamina >= DodgeStaminaCost) {
		Stats->Stamina -= DodgeStaminaCost;
		bIsDodging = true;
	}
}

void APlayerCharacter::DodgingStarted() {
	EMovementState = EMovementState::EMS_Dodging;
	bInvincible = true;
	GetCharacterMovement()->MaxWalkSpeed = 1000.0f;

	if (bTelekinesis) {
		DodgingDirection = UKismetMathLibrary::GetDirectionUnitVector(GetActorForwardVector(), GetVelocity());
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	else DodgingDirection = GetActorForwardVector();

	DodgingTimer.PlayFromStart();
	DodgingTrailComponent->Activate();
	GetWorldTimerManager().ClearTimer(StaminaRegenerationTimer);
}

void APlayerCharacter::DodgingUpdate(const float Alpha) {
	AddMovementInput(DodgingDirection, 1.0f);
	
	UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(DodgingMaterialInterface, GetMesh());
	DynMaterial->SetScalarParameterValue("Dither", Alpha);
	GetMesh()->SetMaterial(0, DynMaterial);
}

void APlayerCharacter::DodgingFinished() {
	bIsDodging = false;
	bInvincible = false;
	EMovementState = EMovementState::EMS_Idle;
	DodgingTrailComponent->Deactivate();
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

#pragma endregion

#pragma endregion

#pragma region Combat

void APlayerCharacter::Attack() {
	if (!bIsDodging) {
		if (!bTelekinesis && EquippedWeapon && Stats->Stamina >= EquippedWeapon->GetStaminaCost() && !EquippedWeapon->GetAttackDelay()) 
			EquippedWeapon->Attack();
		else if (bTelekinesis && (TelekineticPropReference || HighlightedReference)) {
			if (Stats->FocusPoints >= PullFocusCost && ETelekineticAttackState == ETelekinesisAttackState::ETA_None) {
				if (const ITelekinesisInterface* InterfaceFromProp = Cast<ITelekinesisInterface>(HighlightedReference)) {
					// Setting the Telekinesis Prop Reference and Pulling ithttps://www.youtube.com/watch?v=eFRqa-l5OTM
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

void APlayerCharacter::AlternateAttack() {
	if (!bTelekinesis && EquippedWeapon) 
		EquippedWeapon->ChargedAttack();
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

#pragma region Attack Delegate Functions

void APlayerCharacter::OnWeaponAttackStarted() {
	GetWorldTimerManager().ClearTimer(CombatTimer);
	GetWorldTimerManager().ClearTimer(StaminaRegenerationTimer);
	
	Stats->Stamina -= EquippedWeapon->GetStaminaCost();
	bInCombat = true;
}

void APlayerCharacter::OnWeaponAttackEnded() {
	// Stamina
	GetWorldTimerManager().ClearTimer(StaminaRegenerationTimer);
	GetWorldTimerManager().SetTimer(StaminaRegenerationTimer, this, &APlayerCharacter::RegenerateStamina, 0.1f, true, StaminaDelay);
	
	// Combat
	GetWorldTimerManager().ClearTimer(CombatTimer);
	GetWorldTimerManager().SetTimer(CombatTimer, this, &APlayerCharacter::SetCombatState, 1.0f, false, CombatDelay);
}

#pragma endregion

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
		GetWorldTimerManager().SetTimer(HealthRegenerationTimer, this, &APlayerCharacter::RegenerateHealth, HealingDelay, true);
	}
}

void APlayerCharacter::DepleteFocus() {
	Stats->FocusPoints -= ConstantFocusRate * 0.25f;
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
	AVoidSpokenGameModeBase* GM;
	GM = Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GM->StaminaPickup > 0)
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
	else
		return;
}

#pragma endregion 