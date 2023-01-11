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
	// Constructor
	UStatsMasterClass();

	///Will
	// Main Stats
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Health")
	float Health = MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Focus")
	float FocusPoints = MaxFocus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Stamina")
	float Stamina = MaxStamina;
	
	/// Yun
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
		float Defense = 0;
	
	//Runes ***TEMP***
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RUNES")
	int RunePoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RUNES")
	int LesserRune;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RUNES")
	int GreaterRune;

	///Will
	///Player Functions

	//Regen Functions (will be called every frame)
	void RegenHP(float _HP)
	{
		BaseHealth += _HP;
	}
	void RegenFP();
	void RegenStamina(float _ST);

	/// Set & Get Max Stats
	UFUNCTION(BlueprintCallable)
	void SetMaxHealthPlayer();

	UFUNCTION(BlueprintCallable)
	void SetMaxHealth(float MHP);
	
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetMaxHealth()
	{
		return MaxHealth;
	}
	
	// Get/Set MaxFocus
	UFUNCTION(BlueprintCallable)
	void SetMaxFocusPlayer();

	UFUNCTION(BlueprintCallable)
	void SetMaxFocus(float MFocus);
	
	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetMaxFocus()
	{
		return MaxFocus;
	}

	// Get/Set MaxStamina
	UFUNCTION(BlueprintCallable)
	void SetMaxStaminaPlayer();

	UFUNCTION(BlueprintCallable)
	void SetMaxStamina(float MStam);

	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetMaxStamina()
	{
		return MaxStamina;
	}

	/// Set & Get Base Stats

	// Get/Set BaseHealth
	UFUNCTION(BlueprintCallable)
	void SetBaseHealth(float HP)
	{
		BaseHealth = HP;
	}

	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetBaseHealth()
	{
		return BaseHealth;
	}

	// Get/Set BaseFocus
	UFUNCTION(BlueprintCallable)
	void SetBaseFocus(float focus)
	{
		BaseFocus = focus;
	}

	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetBaseFocus()
	{
		return BaseFocus;
	}


	// Get/Set BaseStamina
	UFUNCTION(BlueprintCallable)
	void SetBaseStamina(float st)
	{
		BaseStamina = st;
	}

	UFUNCTION(BlueprintPure, BlueprintCallable)
	float GetBaseStamina()
	{
		return BaseStamina;
	}

	// Initialize Max HP, Stamina, Focus Points
	void InitializeMaxStats();

	// Initialize Stats
	void InitializeMainStats();


	/*UFUNCTION(BlueprintCallable)
	void RegenTimer();*/

	/*UPROPERTY()
	float RegenTime = 1.0f;*/

private:
	
	// Max Stats (initialized on Begin Play)
	UPROPERTY(VisibleAnywhere, Category = "Stats|Health")
		float MaxHealth;

	UPROPERTY(VisibleAnywhere, Category = "Stats|Focus")
		float MaxFocus;

	UPROPERTY(VisibleAnywhere, Category = "Stats|Stamina")
		float MaxStamina;

	// Base Stats
	UPROPERTY(VisibleAnywhere, Category = "Stats|Health")
		float BaseHealth;

	UPROPERTY(VisibleAnywhere, Category = "Stats|Focus")
		float BaseFocus;

	UPROPERTY(VisibleAnywhere, Category = "Stats|Stamina")
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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
