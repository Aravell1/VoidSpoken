// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "Components/InputComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "StatsMasterClass.h"

///Combo System Includes
#include "BaseWeapon.h"

#include "PlayerCharacter.generated.h"


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


	void MoveForward(float Axis);
	void MoveRight(float Axis);

	void RunStart();
	void RunStop();
	bool IsRunning = false;

	float GamepadTurnRate;

	void TurnRate(float Rate);

	void LookUpRate(float Rate);

	/// Dev: YunYun
	/// Adding Weapons to this class
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		//TSubclassOf<ABaseWeapon*> EquippedWeapon = nullptr;
		ABaseWeapon* EquippedWeapon = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float WalkSpeed = 250;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float RunSpeed = 450;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void Attack();
	void UniqueAttack();

};
