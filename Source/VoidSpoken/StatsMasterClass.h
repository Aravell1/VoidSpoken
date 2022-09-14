// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatsMasterClass.generated.h"

//Will
/// <Stats Variables needed for Player>
/// Health points (HP)
/// Focus Points (FP)
/// Stamina (ST)
/// Vitality (VIT)
/// Stength (STR)
/// Intelligence (INT)
/// Endurance (END)
/// Damage (DMG)
/// Rune Points (RP) (same as Skill points)
/// </summary>

//Will
/// <Stats functions needed for Player>
/// HP Regen
/// FP Regen
/// Stamina Regen
/// 
/// Rune Use (maybe?)
/// Rune Points Increase
/// Stats Level Increase (VIT, STR, INT, END)
/// Stats Modifier (HP, FP, Stamina, Damage)
/// </summary>


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class VOIDSPOKEN_API UStatsMasterClass : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatsMasterClass();

	///Will
	///Variables
	// Main Stats
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	float Health = MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	float FocusPoints = MaxFocus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	float Stamina = MaxStamina;
	
	
	//Runes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RUNES")
	int RunePoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LEVELS")
	int LesserRune;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LEVELS")
	int GreaterRune;


	///Will
	///Player Functions

	//Regen Functions (will be called every frame)
	void RegenHP(int _HP);
	void RegenFP(int _FP);
	void RegenStamina(int _ST);


	// Get Max Stats

	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetMaxHealth()
	{
		return MaxHealth;
	}

	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetMaxFocus()
	{
		return MaxFocus;
	}

	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetMaxStamina()
	{
		return MaxStamina;
	}

	// Initialize Max HP, Stamina, Focus Points
	void InitializeMaxStats();

	// Initialize Stats
	void InitializeMainStats();


private:
	
	// Max Stats (initialized on Begin Play)
	float MaxHealth;
	float MaxFocus;
	float MaxStamina;

	// Base Stats
	float BaseHealth;
	float BaseFocus;
	float BaseStamina;

	//Player Levels
	float VitalityLvl;
	float StrenghtLvl;
	float IntelligenceLvl;
	float EnduranceLvl;

	//Level up functions
	void IncreaseVIT();
	void IncreaseSTR();
	void IncreaseINT();
	void IncreaseEND();

	//Runes functions
	void Lesser();
	void Greater();

	// Set Max Stats
	void SetMaxHP();
	void SetMaxFocus();
	void SetMaxStamina();

	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
