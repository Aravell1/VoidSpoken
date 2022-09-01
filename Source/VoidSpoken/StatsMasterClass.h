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
	//Base stats
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	float FocusPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	float Stamina;
	
	
	//Runes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RUNES")
	int RunePoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LEVELS")
	int LesserRune;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LEVELS")
	int GreaterRune;

	// Initialize Max HP, Stamina, Focus Points
	void InitializeMaxStats();

	///Will
	///Player Functions

	//Regen Functions (will be called every frame)
	void RegenHP(int _HP);
	void RegenFP(int _FP);
	void RegenStamina(int _ST);


private:
	
	// Max Stats (initialized on Begin Play)
	//UPROPERTY(BlueprintReadOnly, Category = "STATS")
	float MaxHealth;
	//UPROPERTY(BlueprintReadOnly, Category = "STATS")
	float MaxFocus;
	//UPROPERTY(BlueprintReadOnly, Category = "STATS")
	float MaxStamina;

	//Player Levels
	//UPROPERTY(BlueprintReadOnly, Category = "LEVELS")
	float VitalityLvl;
	//UPROPERTY(BlueprintReadOnly, Category = "LEVELS")
	float StrenghtLvl;
	//UPROPERTY(BlueprintReadOnly, Category = "LEVELS")
	float IntelligenceLvl;
	//UPROPERTY(BlueprintReadOnly, Category = "LEVELS")
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
