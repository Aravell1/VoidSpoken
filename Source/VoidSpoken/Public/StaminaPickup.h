// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TextRenderComponent.h"
#include "StaminaPickup.generated.h"

/**
 * 
 */
UCLASS()
class VOIDSPOKEN_API AStaminaPickup : public ABaseItem
{
	GENERATED_BODY()

public:
	//Constructor
	AStaminaPickup();

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void PickupStamina();
	

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Pickup Text")
	UTextRenderComponent* PickupText;
};
