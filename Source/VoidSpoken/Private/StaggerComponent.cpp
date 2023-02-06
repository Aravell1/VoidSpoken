// Fill out your copyright notice in the Description page of Project Settings.


#include "StaggerComponent.h"

// Sets default values for this component's properties
UStaggerComponent::UStaggerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UStaggerComponent::AddToStaggerGauge(float Damage)
{
	if (bCanGainStagger)
	{
		bDrainStagger = false;

		StaggerGauge += Damage;

		if (StaggerGauge >= StaggerDamageThreshold)
		{
			StaggerOwner();

			StaggerGauge = 0;
			bCanGainStagger = false;

			GetWorld()->GetTimerManager().SetTimer(StaggerTimer,
				this,
				&UStaggerComponent::ResetStaggerCooldown,
				StaggerCooldown);
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimer(StaggerTimer,
				this,
				&UStaggerComponent::StartDrainingStagger,
				StaggerDrainTimer);
		}
	}
}

void UStaggerComponent::StaggerOwner()
{
	StaggerTrigger.Broadcast();
}

void UStaggerComponent::StartDrainingStagger()
{
	bDrainStagger = true;
}

void UStaggerComponent::ResetStaggerCooldown()
{
	bCanGainStagger = true;
}

float UStaggerComponent::GetStaggerGauge()
{
	return StaggerGauge;
}

// Called when the game starts
void UStaggerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UStaggerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bDrainStagger && StaggerGauge > 0)
	{
		StaggerGauge -= StaggerDamageThreshold * PercentStaggerDrainRate * DeltaTime;

		if (StaggerGauge <= 0)
		{
			StaggerGauge = 0;
			bDrainStagger = false;
		}
	}
}

