// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatsMasterClass.h"
#include "Kismet/GameplayStatics.h"
#include "VoidSpoken/VoidSpokenGameModeBase.h"
#include "InventorySystem.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VOIDSPOKEN_API UInventorySystem : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventorySystem();

	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void UseHealthConsumable();

	UFUNCTION(BlueprintCallable)
	void UseFocusConsumable();

	UFUNCTION(BlueprintCallable)
	void UseStaminaConsumable();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stats")
	class UStatsMasterClass* Stats;

private:

	float HealAmount;
	float FocusAmount;
	float StaminaAmount;
	
};
