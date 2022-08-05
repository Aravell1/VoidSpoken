// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GamepadTurnRate = 50.0f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// setup character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->MaxWalkSpeed = 700.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;

	// setup camera
	CameraArm = CreateAbstractDefaultSubobject<USpringArmComponent>(TEXT("Camera arm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->TargetArmLength = 300;
	CameraArm->bUsePawnControlRotation = true;

	FollowCamera = CreateAbstractDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	FollowCamera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	
	/// Set Player base Stats

	
	/// TEMP FIX UNTIL GET/SET FROM MAIN STATS CLASS WORKS PROPERLY ///
	
	// Set Player levels
	VitalityLvl = 1.0f;
	StrenghtLvl = 1.0f;
	IntelligenceLvl = 1.0f;
	EnduranceLvl = 1.0f;

	// Set Player Base stats
	Health = 30.0f;
	FocusPoints = 20.0f;
	Stamina = 50.0f;
	Damage; // Need clarification on base damage

	//Set Player Max Stats
	MaxHealth;
	MaxFocus;
	MaxStamina;

	// Set Player Rune Points
	RunePoints;

	//Set player State if in combat
	bool InCombat = false;
}



// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	//is in combat check here for stats regen calls//
}

// Called to bind functionality to input
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
}


//Finding foward vector and moving
void APlayerCharacter::MoveForward(float Axis)
{
	if ((Controller != nullptr) && (Axis != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		if (!IsRunning)
		{
			Axis *= 0.5f;
		}

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Axis);
	}
}

//Find Rotation
void APlayerCharacter::MoveRight(float Axis)
{
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, Axis);
}

void APlayerCharacter::TurnRate(float Rate)
{
	AddControllerYawInput(Rate * GamepadTurnRate * GetWorld()->GetDeltaSeconds());
}

void APlayerCharacter::LookUpRate(float Rate)
{
	AddControllerPitchInput(Rate * GamepadTurnRate * GetWorld()->GetDeltaSeconds());
}


// Sprint **NEED TO CHANGE FOR A TOGGLE**
void APlayerCharacter::RunStart()
{
	IsRunning = true;
}

void APlayerCharacter::RunStop()
{
	IsRunning = false;
}