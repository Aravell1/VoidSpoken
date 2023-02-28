// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"
#include "StatsMasterClass.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseWeaponInterface.h"
#include "Engine.h"
#include "DamageTypeStagger.h"
#include "Engine/EngineTypes.h"
#include "BaseWeapon.generated.h"

/*
* Script Name:	BaseWeapon.h
* Description:
*		Creating a generic weapon class with required functions and variables. Intended for 
*	creating sub-classes deriving from this class to have their own unique features.
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/

#pragma region Macro Delegate Assignments

DECLARE_DELEGATE(FOnAttackStartedDelegate);
DECLARE_DELEGATE(FOnAttackEndedDelegate);

#pragma endregion

#pragma region Enums

UENUM() 
enum EAttackType {
	EAT_None			UMETA(DisplayName = "None"),
	EAT_NormalAttack	UMETA(DisplayName = "Normal Attack"),
	EAT_ChargedAttack	UMETA(DisplayName = "Charged Attack"),
};

#pragma endregion 

UCLASS(Abstract)
class VOIDSPOKEN_API ABaseWeapon : public AActor, public IBaseWeaponInterface {
	GENERATED_BODY()
	
public:	
	/// Sets default values for this actor's properties
	ABaseWeapon();

	/// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	#pragma region Getter Functions

	/// Function: GetBaseDamage()
	///		-Returns the float BaseDamage
	UFUNCTION(BlueprintPure, BlueprintCallable)
		float GetBaseDamage() { return BaseDamage; };

	/// Function: GetCurrentComboIndex()
	///		-Returns the int CurrentComboIndex
	UFUNCTION(BlueprintPure, BlueprintCallable)
		int GetCurrentComboIndex() { return CurrentComboIndex; };

	/// Function: GetComboLength()
	///		-Returns an int of the Max() of Elements inside the ComboAnimationMontage
	UFUNCTION(BlueprintPure, BlueprintCallable)
		int GetComboLength() { return ComboAnimationMontage.Max(); };

	/// Function: GetEquippedCharacter() 
	///		-Returns a ACharacter* of the variable EquippedCharacter
	UFUNCTION(BlueprintPure, BlueprintCallable)
		ACharacter* GetEquippedCharacter() { return EquippedCharacter; };
	
	/// Function: GetAttackDelay()
	///		-Returns the bool AttackDelay
	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool GetAttackDelay() { return bAttackDelay; };

	/// Function: SetAttackDelay()
	///		-Sets the boolean AttackDelay to {state}
	UFUNCTION(BlueprintCallable)
		void SetAttackDelay(const bool State) { bAttackDelay = State; };

	/// Function: GetAttackDelay()
	///		-Returns the bool AttackDelay
	UFUNCTION(BlueprintPure, BlueprintCallable)
		bool GetIsAttacking() const { return bAttackDelay; };

	/// Function: CheckMovementMode()
	///		-Fetches the current MovementMode from the EquippedCharacter, and compares if it's a valid mode to attack in
	UFUNCTION()
		bool CheckMovementMode() const;

	UFUNCTION() 
		float GetStaminaCost() const {
			if (ComboStaminaMultipliers.IsValidIndex(CurrentComboIndex)) return BaseStamina * ComboStaminaMultipliers[CurrentComboIndex];
			else return BaseStamina;
		}

	#pragma endregion

	#pragma region Weapon Functions

	/// Function: Equip(ACharacter* EquippingCharacter) 
	///		-This functions sets the variable EquippedCharacter
	///		-Will Also set this Weapon's owner as EquippedCharacter
	///		-Initialises the UAnimInstance from the Default (At the time of this Equip(), what was the UAnimInstance the Character had)
	///		-Initialises the UCharacterMovementComponent from the given EquippedCharacter 
	UFUNCTION(BlueprintCallable)
		void Equip_Implementation(ACharacter* EquippingCharacter) override;

	/// Function: Attack()
	///		-Momentarily disables player movement
	///		-Set Timers (AttackDelay) to be active and delays for the respective UAnimMontage's PlayLength()
	///		-Will not play any animations if the ComboAnimationMontage are not set 
	UFUNCTION(BlueprintCallable)
		virtual void Attack();

	/// Function: ChargedAttack()
	///		-Momentarily disables player movement
	///		-Set Timers (AttackDelay) to be active and delays for the respective UAnimMontage's PlayLength()
	///		-Will not play any animations if the ComboAnimationMontage are not set 
	///		-Can be play at any CurrentComboIndex, and will reset the CurrentComboIndex to 0
	UFUNCTION(BlueprintCallable)
		virtual void ChargedAttack();

	/// Function: NextAttack()
	///		-Re-Enables the player movement
	///		-Gets called after Weapon Attack Notify (On End())
	///		-Iterates the CurrentComboCount
	UFUNCTION(BlueprintCallable)
		virtual void NextAttack();

	/// Function: DealDamage()
	///		-Checks if the Attached Box Collider is overlapping any Actors (Preferably if their from a BaseEntity class)
	///		-Length of this function is determined in each of the ComboAnimationMontage
	///		-Uses the Notify and NotifyState systems in each Animation
	UFUNCTION(BlueprintCallable)
		virtual void DealDamage(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent);

	/// Function: Reset() 
	///		-Resets the player back into their Default Animation Instance
	///		-Sets CurrentComboIndex to 0
	///		-Enables Player Movement
	///		-Clears Timers (AttackDelay, ResetDelay)
	///		-Only Use when the player either has been interrupted (via rolling, falling, stunned, etc)
	UFUNCTION(BlueprintCallable)
		virtual void Reset();

	/// Function: Clear()
	///		-Clears the OVerlapped Actors array Mid Attack
	UFUNCTION(BlueprintCallable)
		virtual void Clear();

	#pragma endregion

	protected:
	/// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	#pragma region Weapon Parameters

	/// Base Damage of this Weapon
	///		-Any scaling involving the damage of the Attack, this value will influence
	///		-Can be set to any float value (although not negative, may produce unwanted results)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (NoGetter))
	float BaseDamage = 0;

	/// Base Stamina of this Weapon
	///		-How much each Attack costs within the ComboAttackString
	///		-Cannot Attack if the player does not have enough stamina
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (NoGetter))
	float BaseStamina = 0;

	/// The Actor that has equipped this weapon
	///		-For purposes of unwanted self damage infliction
	///		-This will also be used to equip to this Actor
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ACharacter* EquippedCharacter;

	/// The List of Actors that are being overlapped to make sure we dont hit again.
	///		-This is called everytime a overlap beings
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (NoGetter))
	TArray<AActor*> OverlappedActors = {};

	#pragma endregion

	#pragma region Normal Combo Parameters

	/// This Array contains all the Animation Montages for the Attack of this weapon.
	///		-This cannot be uninitialized
	///		-This will loop constantly when this runs through the whole Montage Array
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Normal Attack")
		TArray<UAnimMontage*> ComboAnimationMontage = {};

	/// This array will dictate how much of the Base Damage is accounted for
	///		-Normalized Value (%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Normal Attack")
		TArray<float> ComboAttackMultipliers = {};

	/// This array will scale how much stamina the attack (within the array) is used
	///		-Normalized Values (%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Normal Attack")
		TArray<float> ComboStaminaMultipliers = {};
	
	/// This keeps track of the current Attack to be executed.
	///		-Cannot be edited by blueprints to prevent any unwanted behaviours
	///		-Will only increment to the length of the ComboMontage - 1
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (NoGetter), Category = "Normal Attack")
		int CurrentComboIndex = 0;

	#pragma endregion

	#pragma region Charged Attack

	/// This Charged Attack Animation will be played if the Entity uses it
	///		-This parameter can be uninitialized
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Charged Attack")
		UAnimMontage* ChargedAttackMontage = nullptr;

	/// This value will dictate how much of the Base Damage is accounted for
	///		-Normalized Value (%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Charged Attack")
		float ChargedAttackComboMultiplier = 0;

	/// How much Stamina is comsumed
	///		-Will not player if Player doesn't have enough resources
	///		-This is a subtractive value
	///		-This value substracts from the Stats->Stamina
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Charged Attack")
		float ChargedAttackConsumption = 0;

	#pragma endregion

	#pragma region Components of this class

	public:
	
	/// Mesh of this weapon
	///		-Can be uninitialized, although not recommended
	///		-Collision will be based off of this mesh (Simple Collision)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* WeaponMeshComponent = nullptr;

	UPROPERTY(VisibleAnywhere, DisplayName = "Weapon Material")
	UMaterialInterface* WeaponMaterialInterface;
	
	/// Collision of this Weapon
	///		-This is a box that attaches to a socket on the mesh itself
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		class UBoxComponent* WeaponCollisionBox = nullptr;
	
	///Equipped Actors movement component to be set disabled and re-enabled when attacking
	UPROPERTY(BlueprintReadOnly)
		class UCharacterMovementComponent* EquippedCharacterMovementComponent = nullptr;

	#pragma endregion

	#pragma region Combo States and Booleans

	public:

	/// This keeps track of the current Attack to be executed.
	///		-Cannot be edited by blueprints to prevent any unwanted behaviours
	///		-Will only increment to the length of the ComboMontage - 1
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (NoGetter))
		TEnumAsByte<EAttackType> EAttackState = EAT_None;

	/*
	/// Boolean for when the attack started (prevent spamming attack inputs)
	UPROPERTY(VisibleAnywhere, DisplayName = "Is Attacking")
		bool bIsAttacking = false;
	 */

	/// Boolean for Checking Collisions for the weapon to deal Damage
	UPROPERTY(VisibleAnywhere, DisplayName = "Check For Overlapped Actors")
		bool bCheckForOverlappedActors = false;

	UFUNCTION(BlueprintCallable)
	void SetCheckForOverlappedActors(bool State) { bCheckForOverlappedActors = State; };

	/// Boolean Handler for NextAttack(), and to prevent the player attacking again
	UPROPERTY(VisibleAnywhere, DisplayName = "Attacking Delay")
		bool bAttackDelay = false;

	#pragma endregion

	#pragma region Collision Delegates
	
	UFUNCTION()
		virtual void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	#pragma endregion

	#pragma region Delegates and Events

	public:
	FOnAttackStartedDelegate OnAttackStarted;
	FOnAttackEndedDelegate OnAttackEnded;

	#pragma endregion
};
