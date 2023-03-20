// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TelekinesisInterface.h"
#include "Components/TimelineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerCharacter.h"
#include "DamageTypeTelekinesis.h"
#include "TelekineticProp.generated.h"

UCLASS(Blueprintable)
class VOIDSPOKEN_API ATelekineticProp : public AActor, public ITelekinesisInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATelekineticProp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* StaticMesh = nullptr;

	UPROPERTY(BlueprintReadWrite)
		ACharacter* PlayerCharacter = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		ETelekinesisState State = ETelekinesisState::ETS_Default;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float PropDamage = 0;
	
	FTimerHandle SimulatePhysicsDelay;
	FTimerHandle EnableGravityDelay;

	void ToggleSimulatePhysics() {
		if (StaticMesh->GetComponentVelocity().Size() <= 0.5f) {
			StaticMesh->SetSimulatePhysics(false);
			GetWorldTimerManager().ClearTimer(SimulatePhysicsDelay);
		}
	}

	#pragma region Liftting Functions and Parameters
	
	UPROPERTY(BlueprintReadWrite)
		FVector LiftStart;

	UPROPERTY(BlueprintReadWrite)
		FVector LiftEnd;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		float LiftHeight = 100.0f;

	FTimeline LiftTimeline;

	UPROPERTY(BlueprintReadWrite)
		FVector PushTarget;

	#pragma region Visual Effects Prototypes

	UFUNCTION()
		void ReachCharacter();

	UFUNCTION()
		void ChaoticJitter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		uint8 JitterCounter = 0;

	UFUNCTION()
		void LiftUpdate(const float Alpha);

	UFUNCTION()
		void LiftFinished();

	UPROPERTY(BlueprintReadWrite)
		UCurveFloat* LiftCurve = nullptr;

	#pragma endregion

	#pragma endregion

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	/// Implementing the Telekinesis interface
	void Highlight_Implementation(bool bHighlight) override;

	void Pull_Implementation(ACharacter* Character) override;

	void Push_Implementation(FVector Destination, float Force) override;

	void Drop_Implementation() override;

	// Visual Effects Functions
	void LiftOff();

	void StopLift();

	void SetGravity() const { StaticMesh->SetEnableGravity(bEnableGravityDelay); }

	UPROPERTY(EditDefaultsOnly, DisplayName = "Enable Stupid Feature")
		bool bEnableGravityDelay = false;

	bool bCanBeLifted = true;

	void LerpAngularVelocityToZero(float DeltaSeconds);
	bool bLerpAngularVelocity = false;
	const float LerpDuration = 0.5f;
	float LerpTimer = 0;
	FVector StartVelocity = FVector::ZeroVector;

	// Collision Events

	UFUNCTION()
		void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
};
