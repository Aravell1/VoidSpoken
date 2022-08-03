// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "StatsMasterClass.h"
#include "PlayerCharacter.generated.h"

UStatsMasterClass PlayerStats;

UCLASS()
class VOIDSPOKEN_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
		class UCameraComponent* FollowCamera;

	void MoveForward(float Axis);
	void MoveRight(float Axis);

	void RunStart();
	void RunStop();
	bool IsRunning = false;

	float GamepadTurnRate;

	void TurnRate(float Rate);

	void LookUpRate(float Rate);

	/// Player stats

	// Setting Player Stats to be used in BP

	
	UFUNCTION(BlueprintCallable)
	FFloat16 ReturnVitality()
	{
		return PlayerStats.VitalityLvl;
	}

	UFUNCTION(BlueprintCallable)
	FFloat16 ReturnSTR()
	{
		return PlayerStats.StrenghtLvl;
	}

	UFUNCTION(BlueprintCallable)
	FFloat16 ReturnIntelligence()
	{
		return PlayerStats.IntelligenceLvl;
	}
	
	UFUNCTION(BlueprintCallable)
	FFloat16 ReturnEndurence()
	{
		return PlayerStats.EnduranceLvl;
	}
	
	UFUNCTION(BlueprintCallable)
	FFloat16 ReturnHealth()
	{
		return PlayerStats.Health;
	}

	UFUNCTION(BlueprintCallable)
	FFloat16 ReturnFocus()
	{
		return PlayerStats.FocusPoints;
	}
	
	UFUNCTION(BlueprintCallable)
	FFloat16 ReturnStamina()
	{
		return PlayerStats.Stamina;
	}
	
	UFUNCTION(BlueprintCallable)
	FFloat16 ReturnDamage()
	{
		return PlayerStats.Damage;
	}
	// Need clarification on base damage

	UFUNCTION(BlueprintCallable)
	FFloat16 ReturnMaxHealth()
	{
		return PlayerStats.MaxHealth;
	}

	UFUNCTION(BlueprintCallable)
		FFloat16 ReturnMaxFocus()
	{
		return PlayerStats.MaxFocus;
	}

	UFUNCTION(BlueprintCallable)
		FFloat16 ReturnMaxStamina()
	{
		return PlayerStats.MaxStamina;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
