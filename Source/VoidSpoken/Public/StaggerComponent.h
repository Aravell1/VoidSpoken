// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaggerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStaggerDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VOIDSPOKEN_API UStaggerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStaggerComponent();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bCanStagger = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float StaggerDamageThreshold = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float StaggerCooldown = 5.0f;

	void AddToStaggerGauge(float Damage);

	void StaggerOwner();

	void StartDrainingStagger();

	void ResetStaggerCooldown();

	UFUNCTION(BlueprintPure)
		float GetStaggerGauge();
	
	bool bCanGainStagger = true;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FStaggerDelegate StaggerTrigger;

private:

	FTimerHandle StaggerTimer;

	float StaggerGauge = 0;
	const float StaggerDrainTimer = 2.5f;
	const float PercentStaggerDrainRate = 0.2f;
	bool bDrainStagger = false;
		
};
