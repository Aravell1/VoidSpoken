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
	///Variables for player
	//Base stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	int Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	int FocusPoints;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	int Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	int Damage;
	

	//Player Stat Levels
	int VitalityLvl;
	int StrenghtLvl;
	int IntelligenceLvl;
	int EnduranceLvl;


	//Runes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "STATS")
	int RunePoints;
	
	int LesserRune;
	int GreaterRune;

	///Will
	//Player Functions

	//Regen Functions
	void RegenHP(float _HP);
	void RegenFP(float _FP);
	void RegenStamina(float _ST);
	
	//Level up functions
	void IncreaseVIT(int _Vit);
	void IncreaseSTR(int _Str);
	void IncreaseINT(int _Int);
	void IncreaseEND(int _End);

	//Stats Modifiers
	///may not need, keeping in case of change
	/*void ModifyHP(float _Health);
	void ModifyFP(float _Focus);
	void ModifyStamina(float _Stamina);
	void ModifyDamage(float _Damage);*/

	///Will
	//Runes functions
	void Lesser(int _Les);
	void Greater(int _Grt);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
