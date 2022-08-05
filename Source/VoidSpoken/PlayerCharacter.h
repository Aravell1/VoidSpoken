// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PlayerCharacter.generated.h"


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

	/// TEMP FIX UNTIL GET/SET FROM MAIN STATS CLASS WORKS PROPERLY ///

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	float VitalityLvl;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	float StrenghtLvl;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	float IntelligenceLvl;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	float EnduranceLvl;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	float FocusPoints;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	float Stamina;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	float Damage;
	// Need clarification on base damage

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	float MaxHealth;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	float MaxFocus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	int RunePoints;
	
	////////////////////////////////////////////////////////////////


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
