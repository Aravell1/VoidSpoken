// Fill out your copyright notice in the Description page of Project Settings.

/*
* Script Name:	PlayerCharacter.h
* Description:
*		Creating te player character and controller for the player to use, this class connects all the functionality
*	to the player such as telekinesis 
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/

#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "VoidSpoken/VoidSpokenGameModeBase.h"
#include "BaseEntity.h"
#include "TimerManager.h"
#include "Engine/EngineTypes.h"
#include "StatsMasterClass.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Components/TimelineComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "TelekinesisInterface.h"

#include "UObject/UObjectGlobals.h"

#include "PlayerCharacter.generated.h"

#pragma region ENums

UENUM(BlueprintType)
enum class ETelekinesisAttackState : uint8 {
	ETA_None = 0 UMETA(DisplayName = "None"),	// Holding Nothing
	ETA_Pull = 1 UMETA(DisplayName = "Pull"),	// Started Pulling an Object (Can stop mid way through)
	ETA_Hold = 2 UMETA(DisplayName = "Hold"),	// Got to holding position and awaiting to be thrown
	ETA_Push = 3 UMETA(DisplayName = "Push"),	// Pushing the thrown telekinetic object
};

UENUM(BlueprintType)
enum class EMovementState : uint8 {
	EMS_Idle = 0	UMETA(DisplayName = "Idle"),		// Idle
	EMS_Walking = 1 UMETA(DisplayName = "Walking"),		// Walking
	EMS_Running = 2 UMETA(DisplayName = "Running"),		// Running
	EMS_Dodging = 3 UMETA(DisplayName = "Dodging"),		// Dodging
};

#pragma endregion

class ACombatDirector;
class ABaseWeapon;

UCLASS()
class VOIDSPOKEN_API APlayerCharacter : public ABaseEntity {
	GENERATED_BODY()

public:
	APlayerCharacter();

	#pragma region Camera Functions
	
	protected:

	bool bGodMode = false;
	
	float GamepadTurnRate = 50.0f;

	void TurnRate(const float Rate);
	void LookUpRate(const float Rate);

	UFUNCTION(BlueprintCallable)
	void ResetCameraRotation();

	#pragma endregion

	#pragma region Weapons and Functions

	public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Active Equipped Weapon")
	ABaseWeapon* EquippedWeapon = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Left Weapon")
	ABaseWeapon* LeftEquippedWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon|Right Weapon")
	ABaseWeapon* RightEquippedWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TArray<TSubclassOf<ABaseWeapon>> WeaponInventory = {};

	UFUNCTION(BlueprintCallable)
	void EquipFromInventory(int32 Index, FName EquippingSocket);

	UFUNCTION(BlueprintPure, BlueprintCallable)
	bool GetInCombat() const { return bInCombat; }

	UPROPERTY(VisibleAnywhere, DisplayName = "Combat Director")
	ACombatDirector* CombatDirector = nullptr;

	#pragma region Combat Booleans and Timers
	
	protected:
	
	/// Determines if the player can be damaged
	UPROPERTY(VisibleAnywhere, Category = "Stats|Combat", DisplayName = "In Combat")
	bool bInCombat = false;
	
	UPROPERTY(VisibleAnywhere, Category = "Stats|Combat", DisplayName = "Is Attacking")
	bool bIsAttacking = false;

	#pragma region Timers and Variables
	
	/// The delay before the player returns to a non-combat state (Outside of combat)
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Combat", DisplayName = "Combat Delay (s)")
	float CombatDelay = 5.0f;
	
	void SetCombatState() {
		bInCombat = !bInCombat;
		
		if (!bInCombat) {
			if (LeftEquippedWeapon) LeftEquippedWeapon->Hide();
			if (RightEquippedWeapon) RightEquippedWeapon->Hide();
		}
	};

	FTimerHandle CombatTimer;

	#pragma endregion

	#pragma endregion

	#pragma endregion
	
	#pragma region Components
	
	public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* TelekinesisSource = nullptr; // FVector(-190, 40, 147)

	#pragma endregion 

	#pragma region Default/Inherited ACharacter Functions
	
	protected:
	virtual void BeginPlay() override;

	FVector GetPlayerForwardDirection() const {
		return FRotationMatrix(FRotator(0, Controller->GetControlRotation().Yaw, 0)).GetUnitAxis(EAxis::X);
	};

	#pragma endregion

	#pragma region Player Movement States and Variables

	public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetRunning() { return bIsRunning; }
	
	protected:

	UPROPERTY(VisibleAnywhere)
	FVector PlayerInput;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	FVector GetPlayerInput() { return PlayerInput; }
	
	void MoveForward(float Axis);
	void MoveRight(float Axis);

	void RunStart();
	void RunStop();
	bool bIsRunning = false;

	/// Controls the available movement restrictions and keeps track of the player's current actions
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName = "Movement State")
	EMovementState EMovementState = EMovementState::EMS_Idle;

	UFUNCTION(BlueprintCallable)
	void DetermineMovementState();

	/// Determines how fast the Walking Speed the player moves
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName = "Walking Speed")
	float WalkSpeed = 350.0f;
	
	/// Determines how fast the Running Speed the player moves
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName = "Run Speed")
	float RunSpeed = 450.0f;

	#pragma endregion 

	#pragma region Telekinesis Variables and Prototypes

	#pragma region Getter / Setter

	public:

	UFUNCTION(BlueprintCallable)
	void SetGodMode() { bGodMode = !bGodMode; }

	UFUNCTION(BlueprintPure)
	bool GetGodModeEnabled() { return bGodMode; }

	UFUNCTION()
	void SetTelekineticAttackState(const ETelekinesisAttackState State) { ETelekineticAttackState = State; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
	ETelekinesisAttackState GetTelekineticAttackState() const { return ETelekineticAttackState; };

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetTelekinesis() const { return bTelekinesis; };

	#pragma endregion
	
	protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName = "Telekinetic Attack State")
	ETelekinesisAttackState ETelekineticAttackState = ETelekinesisAttackState::ETA_None;

	UPROPERTY(EditAnywhere, Category = "Telekinetic Abilities", DisplayName = "Telekinesis")
	bool bTelekinesis = false;

	UPROPERTY(VisibleAnywhere, Category = "Telekinetic Abilities")
	AActor* HighlightedReference = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Telekinetic Abilities")
	AActor* TelekineticPropReference = nullptr;

	/// Determines how hard the "Push" Telekinetic ability can push a TelekineticProp object
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Telekinetic Abilities", DisplayName = "Push Force")
	float PushForce = 250.0f;
	
	/// Determines how large the SphereTrace of detecting TelekineticProps are
	///		- Making it too large can make the player start constantly being triggered
	UPROPERTY(EditDefaultsOnly, Category = "Telekinetic Abilities", DisplayName = "Detection Radius")
	float DetectionRadius = 25.0f;

	/// Determines the max distance the SphereTrace can travel
	UPROPERTY(EditAnywhere, Category = "Telekinetic Abilities" , DisplayName = "Maximum Telekinetic Range")
		float TelekineticRange = 2500.0f;

	UPROPERTY(EditAnywhere, Category = "Telekinetic Abilities", DisplayName = "Minimum Telekinetic Range")
		float MinTelekineticRange = 200.0f;

	#pragma region Timelines and Curve Variables
	
	FTimeline ZoomTimeline;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Zoom Settings")
		UCurveFloat* ZoomCurve = nullptr;

	UFUNCTION()
	void ZoomUpdate(const float Alpha) const;

	#pragma endregion

	/// This is the position of the camera to lerp to when using telekinesis/in-combat
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Zoom Settings")
	FVector CameraOffset = FVector(0, 90, 0);

	UFUNCTION()
	void SetCameraOffset() const;

	/// Input Scalar Values (Sensitivity)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Zoom Settings", DisplayName = "Input Yaw Scale")
	float InputYawScale = 2.5f;

	#pragma region Telekinetic Function Prototypes
	
	UFUNCTION()
	void HandleTelekinesis();

	UFUNCTION()
	void TelekineticStart();

	UFUNCTION()
	void TelekineticEnd();

	UFUNCTION()
	void DetectTelekineticObject();

	#pragma endregion

	#pragma endregion

	#pragma region Default/Inherited ACharacter Functions

	public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) override;

	#pragma endregion 

	#pragma region Weapon Attacks and Delegates
	
	void LeftAttack();
	void RightAttack();

	void OnWeaponAttackStarted();
	void OnWeaponAttackEnded();

	#pragma endregion

	#pragma region Consumables / Item Usage

	UFUNCTION(BlueprintCallable)
	void UseHealthConsumable();

	UFUNCTION(BlueprintCallable)
	void UseFocusConsumable();

	UFUNCTION(BlueprintCallable)
	void UseStaminaConsumable();

	UPROPERTY()
	float HealAmount;

	UPROPERTY()
	float FocusAmount;

	UPROPERTY()
	float StaminaAmount;

	#pragma endregion

	#pragma region Pickup Interaction

	public:

	void On_F_Down();
	void On_F_Release();

	bool bIsFDown;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Items")
	class ABaseItem* OverlappingItem;

	FORCEINLINE void SetOverlappingItem(ABaseItem* OverlapItem)
	{
		OverlappingItem = OverlapItem;
	}

	#pragma endregion

	#pragma region Invincible and Health Regeneration
	public:

	/// The delay before the player can start regenerating health (Outside of combat)
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Health", DisplayName = "Healing Delay (s)")
		float HealingDelay = 5.0f;

	/// The rate on how much the player heals, Constant Rate (/s)
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Health", DisplayName = "Healing Rate ( /s )")
		float HealingRate = 2.5f;

	UPROPERTY(BlueprintReadWrite)
		UAnimMontage* HitMontage = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats", DisplayName = "Is Dead")
	bool bIsDead = false;
	
	UFUNCTION(BlueprintNativeEvent)
	void Death();

	/// Flips the bool Invincible
	UFUNCTION(BlueprintCallable)
	bool GetInvincible() { return bInvincible; }
	void SetInvincibility() { bInvincible = !bInvincible; }
	void SetInvincibility(const bool State) { bInvincible = State; }

	protected:
	void OnStaggered() override;
	void RegenerateHealth();
	FTimerHandle InvincibilityTimer;
	FTimerHandle HealthRegenerationTimer;

	/// Determines if the player can be damaged
	UPROPERTY(VisibleAnywhere, Category = "Stats", DisplayName = "Invincible")
	bool bInvincible = false;

	UPROPERTY(VisibleAnywhere, Category = "Stats", DisplayName = "Is Damaged")
	bool bIsDamaged = false;

	UFUNCTION()
	void ResetInvincibility() {
		bInvincible = false;
		if (GetCharacterMovement()->MovementMode == MOVE_None)
			GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}

	#pragma endregion

	#pragma region Focus Costs and Depletion
	
	public:
	
	/// The Cost of the Telekinetic "Pull" Ability, Flat Rate
	UPROPERTY(EditDefaultsOnly, Category = "Telekinetic Abilities|Costs and Consumptions", DisplayName = "Pull Focus Cost")
	float PullFocusCost = 2.0f; // Flat Rate

	/// The Cost of the Telekinetic "Holding" Ability, Constant Rate (/s)
	UPROPERTY(EditDefaultsOnly, Category = "Telekinetic Abilities|Costs and Consumptions", DisplayName = "Holding Focus Rate ( /s )")
	float ConstantFocusRate = 0.75f; // Depletion Rate (1/s)

	/// The Cost of the Telekinetic "Push" Ability, Flat Rate
	UPROPERTY(EditDefaultsOnly, Category = "Telekinetic Abilities|Costs and Consumptions", DisplayName = "Push Focus Cost")
	float PushFocusCost = 4.0f; // Flat Rate

	protected:

	FTimerHandle FocusDepletionTimer;
	void DepleteFocus();
	void RegenerateFocus(float DeltaSeconds);

	float FocusToRegen = 0;
	float FocusRegenRate = 10;
	bool bRegenerateFocus = false;
	#pragma endregion

	#pragma region Stamina Depletion / Regeneration
	
	public:
	void StartFocusRegen();

	/// A delay to start regenerating stamina
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Stamina", DisplayName = "Stamina Delay (s)")
	float StaminaDelay = 0.75f;

	/// The regeneration rate of Stamina, Constant Rate (/s)
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Stamina", DisplayName = "Stamina Regeneration Rate ( /s )")
	float StaminaRate = 11.0f;

	/// The Cost of the Running, Constant Rate (/s)
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Stamina", DisplayName = "Running Stamina Depletion Rate ( /s )")
	float RunningStaminaDepletionRate = 1.5f;

	protected:
	void RegenerateStamina();
	void DepleteStamina();
	FTimerHandle StaminaRegenerationTimer;

	#pragma region Dodging

	public:

	/// The amount of Stamina consumed when dodging, Flat Rate
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Stamina", DisplayName = "Dodge Stamina Cost")
	float DodgeStaminaCost = 8.0f;

	void SetDodging() { bIsDodging = !bIsDodging; if (!bIsDodging) DodgingTimer.Stop(); };
	void SetDodging(const bool State) { bIsDodging = State; };
	
	UFUNCTION()
	void DodgingStarted();

	UFUNCTION()
	void DodgingFinished();
	
	UFUNCTION(BlueprintCallable)
	void ResetDodging();

	protected:

	#pragma region VFXs

	UPROPERTY(VisibleAnywhere, Category = "Dodging", DisplayName = "Dodging Material")
	UMaterialInterface* DodgingMaterialInterface;

	#pragma endregion 
	
	/// Determines if the player is currently dodging, making the player invincible for a short duration of time
	UPROPERTY(VisibleAnywhere, Category = "Dodging", DisplayName = "Is Dodging")
	bool bIsDodging = false;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetIsDodging() const { return bIsDodging; };

	/// A Curve Float object determining smoothness and intensity of the Dodge's distance
	UPROPERTY(EditDefaultsOnly, Category = "Dodging", DisplayName = "Dodge Float Curve")
	UCurveFloat* DodgingCurve = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Dodging", DisplayName = "Dodge Animation")
	UAnimMontage* DodgeAnimation = nullptr;
	
	FTimeline DodgingTimer;	// For Lerping to a point for the dodging animation
	void Dodge();

	UFUNCTION()
	void DodgingUpdate(const float Alpha);

	FVector DodgingDirection;

	/// Determines the max distance the dodge can change the players position
	UPROPERTY(EditDefaultsOnly, Category = "Dodging", DisplayName = "Dodging Distance")
	float DodgeDistance = 750.0f;

	#pragma endregion
	
	#pragma endregion

};
