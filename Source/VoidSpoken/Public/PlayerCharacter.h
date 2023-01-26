// Fill out your copyright notice in the Description page of Project Settings.

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
#include "BaseWeapon.h"

#include "UObject/UObjectGlobals.h"

#include "PlayerCharacter.generated.h"

UENUM()
enum class ETelekinesisAttackState : uint8 {
	ETA_None = 0 UMETA(DisplayName = "None"),	// Holding Nothing
	ETA_Pull = 1 UMETA(DisplayName = "Pull"),	// Started Pulling an Object (Can stop mid way through)
	ETA_Hold = 2 UMETA(DisplayName = "Hold"),	// Got to holding position and awaiting to be thrown
};

UENUM()
enum class EWeaponSlot : uint8 {
	EWS_None = 0 UMETA(DisplayName = "None"),			// Holding Nothing
	EWS_First = 1 UMETA(DisplayName = "First Weapon"),	// Started Pulling an Object (Can stop mid way through)
	EWS_Second = 2 UMETA(DisplayName = "Second Weapon")	// Got to holding position and awaiting to be thrown
};

UENUM()
enum class EMovementState : uint8 {
	EMS_Idle = 0 UMETA(DisplayName = "Idle"),		// Idle
	EMS_Walking = 1 UMETA(DisplayName = "Walking"),	// Walking
	EMS_Running = 2 UMETA(DisplayName = "Running"),	// Running
	EMS_Dodging = 3 UMETA(DisplayName = "Dodging"),	// Dodging
};

UCLASS()
class VOIDSPOKEN_API APlayerCharacter : public ABaseEntity
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* FollowCamera;

	class UCapsuleComponent* PlayerCapsule;
	class USkeletalMeshComponent* PlayerMesh;

	#pragma region Movement / Camera Functions and Variables
private:
	void MoveForward(float Axis);
	void MoveRight(float Axis);

	void RunStart();
	void RunStop();
	bool bIsRunning = false;

	float fGamepadTurnRate;

	void TurnRate(float Rate);

	void LookUpRate(float Rate);

#pragma endregion

public:

	/// Dev: YunYun
	/// Adding Weapons to this class

	#pragma region Weapons and Functions

	EWeaponSlot CurrentWeapon = EWeaponSlot::EWS_None;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Weapon")
		ABaseWeapon* EquippedWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		TArray<TSubclassOf<ABaseWeapon>> WeaponInventory = {};

	UFUNCTION(BlueprintCallable)
		void EquipFromInventory(int32 Index, FName EquippingSocket);

	UFUNCTION(BlueprintCallable)
		void SwapWeapons();

	#pragma endregion

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* TelekinesisSource = nullptr; // FVector(-190, 40, 147)

	UFUNCTION()
		void SetTelekineticAttackState(ETelekinesisAttackState state);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool GetTelekineticState() { return bTelekinesis; };

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/// Controls the available movement restrictions and keeps track of the player's current actions
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName = "Movement State")
		EMovementState EMovementState = EMovementState::EMS_Idle;

	UFUNCTION(BlueprintCallable)
		void DetermineMovementState();

	/// Determines how fast the Walking Speed the player moves
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName = "Walking Speed")
		float fWalkSpeed = 250.0f;
	
	/// Determines how fast the Running Speed the player moves
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName = "Run Speed")
		float fRunSpeed = 450.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, DisplayName = "Telekinetic Attack State")
		ETelekinesisAttackState ETelekineticAttackState = ETelekinesisAttackState::ETA_None;

	#pragma region Telekinesis Variables and Prototypes

	UPROPERTY(EditAnywhere, Category = "Telekinetic Abilities", DisplayName = "Telekinesis")
		bool bTelekinesis = false;

	UPROPERTY(VisibleAnywhere, Category = "Telekinetic Abilities")
		AActor* HighlightedReference = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Telekinetic Abilities")
		AActor* TelekineticPropReference = nullptr;

	/// Determines how hard the "Push" Telkinetic ability can push a TelekineticProp object
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, DisplayName = "Push Force")
		float fPushForce = 450.0f;
	
	/// Determines how large the SphereTrace of detecting TelekineticProps are
	///		- Making it too large can make the player start constatly being triggered
	UPROPERTY(EditDefaultsOnly, Category = "Telekinetic Abilities", DisplayName = "Detection Radius")
		float fDetectionRadius = 25.0f;

	/// Determines the max distance the SphereTrace can travel
	UPROPERTY(EditAnywhere, Category = "Telekinetic Abilities" , DisplayName = "Telekinetic Range")
		float fTelekineticRange = 250000.0f;

	#pragma region Timelines and Curve Variables
	
	FTimeline ZoomTimeline;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Zoom Settings")
		UCurveFloat* ZoomCurve = nullptr;

	UFUNCTION()
		void ZoomUpdate(const float Alpha);

	UFUNCTION()
		void ZoomFinished();

	#pragma endregion

	/// This is the position of the camera to lerp to when using telekinesis/in-combat
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Zoom Settings")
		FVector CameraOffset = FVector(0, 90, 0);

	UFUNCTION()
		void SetCameraOffset();

	/// Input Scalar Values (Sensitivity)
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera|Zoom Settings", DisplayName = "Input Yaw Scale")
		float fInputYawScale = 2.5f;

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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Attack();
	void AlternateAttack();

	void OnWeaponAttackStarted();
	void OnWeaponAttackEnded();

	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser) override;

	UFUNCTION()
		void ResetInvincibility() { bInvincible = false; };

	#pragma region Health Regeneration
	public:

	/// The delay before the player can start regenerating health (Outside of combat)
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Health", DisplayName = "Healing Delay (s)")
		float fHealingDelay = 5.0f;

	/// The rate on how much the player heals, Constant Rate (/s)
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Health", DisplayName = "Healing Rate ( /s )")
		float fHealingRate = 2.5f;

	/// Flips the bool Invincible
	void SetInvincibility() { bInvincible = !bInvincible; };
	void SetInvincibility(bool State) { bInvincible = State; };

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

	private:
	void RegenerateHealth();
	FTimerHandle InvincibilityTimer;
	FTimerHandle HealthRegenerationTimer;

	/// Determines if the player can be damaged
	UPROPERTY(VisibleAnywhere, Category = "Stats", DisplayName = "Invincible")
		bool bInvincible = false;

	#pragma endregion

	#pragma region Focus
	public:
	
	/// The Cost of the Telekinetic "Pull" Ability, Flat Rate
	UPROPERTY(EditDefaultsOnly, Category = "Telekinetic Abilities|Costs and Consumptions", DisplayName = "Pull Focus Cost")
		float fPullFocusCost = 5.0f; // Flat Rate

	/// The Cost of the Telekinetic "Holding" Ability, Constant Rate (/s)
	UPROPERTY(EditDefaultsOnly, Category = "Telekinetic Abilities|Costs and Consumptions", DisplayName = "Holding Focus Rate ( /s )")
		float fConstantFocusRate = 0.75f; // Depleation Rate (1/s)

	/// The Cost of the Telekinetic "Push" Ability, Flat Rate
	UPROPERTY(EditDefaultsOnly, Category = "Telekinetic Abilities|Costs and Consumptions", DisplayName = "Push Focus Cost")
		float fPushFocusCost = 7.5f; // Flat Rate

	private:

	FTimerHandle FocusDepletionTimer;
	void DepleteFocus();

	#pragma endregion

	#pragma region Stamina Depletion / Regeneration
	public:

	/// A delay to start regenerating stamina
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Stamina", DisplayName = "Stamina Delay (s)")
		float fStaminaDelay = 0.75f;

	/// The regeneration rate of Stamina, Constant Rate (/s)
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Stamina", DisplayName = "Stamina Regeneration Rate ( /s )")
		float fStaminaRate = 2.5f;

	/// The Cost of the Running, Constant Rate (/s)
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Stamina", DisplayName = "Running Stamina Depletion Rate ( /s )")
		float fRunningStaminaDepletionRate = 2.5f;

	private:
	void RegenerateStamina();
	void DepleteStamina();
	FTimerHandle StaminaRegenerationTimer;

	#pragma region Dodging

	public:

	/// The amount of Stamina consumbed when dodging, Flat Rate
	UPROPERTY(EditDefaultsOnly, Category = "Stats|Stamina", DisplayName = "Dodge Stamina Cost")
		float fDodgeStaminaCost = 8.0f;

	void SetDodging() { bIsDodging = !bIsDodging; if (!bIsDodging) DodgingTimer.Stop(); };
	void SetDodging(bool State) { bIsDodging = State; };

	void HandleDodge();
	
	UFUNCTION()
		void DodgingStarted();

	UFUNCTION()
		void DodgingFinished();

	private:

	/// Determines if the player is currently dodging, making the player invincible for a short duration of time
	UPROPERTY(VisibleAnywhere, Category = "Dodging", DisplayName = "Is Dodging")
		bool bIsDodging = false;

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool GetIsDodging() { return bIsDodging; };

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
		float fDodgeDistance = 750.0f;

	#pragma endregion
	
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

};
