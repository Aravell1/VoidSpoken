// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "PlayerCharacter.h"
#include "Components/TextRenderComponent.h"
#include "Components/BoxComponent.h"
#include "FocusPickup.generated.h"

/**
 * 
 */
UCLASS()
class VOIDSPOKEN_API AFocusPickup : public ABaseItem
{
	GENERATED_BODY()

public:
	//Constructor
	AFocusPickup();

	virtual void BeginPlay() override;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	void PickupFocus();
};
