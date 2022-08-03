// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsMasterClass.h"

// Sets default values for this component's properties
UStatsMasterClass::UStatsMasterClass()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	//Set base Stats Values

	//Will
	//Player base stats
	VitalityLvl = 1;
	StrenghtLvl = 1;
	IntelligenceLvl = 1;
	EnduranceLvl = 1;

	Health = 30;
	FocusPoints = 20;
	Stamina = 50;
	Damage;

}


// Called when the game starts
void UStatsMasterClass::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UStatsMasterClass::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


//Stats Regeneration

/// <NAME>
/// <Health Regeneration>
/// if player is NOT in combat
/// Regen 2HP each 5 second
/// 
/// </summary>
/// <param name="_HP"></param>
void UStatsMasterClass::RegenHP(float _HP)
{
}

/// <NAME>
/// <Focus Regeneration?>
/// ???
/// </summary>
/// <param name="_FP"></param>
void UStatsMasterClass::RegenFP(float _FP)
{
}

/// <NAME>
/// <Stamina regeneration>
/// if player IS in combat
/// Regen 2 ST each 5 second
/// 
/// OR
/// 
/// if player is NOT in combat
/// Regen 5 ST each 5 second
/// 
/// </summary>
/// <param name="_ST"></param>
void UStatsMasterClass::RegenStamina(float _ST)
{
}



//Leveling

/// <Will>
/// <Level up VIT>
/// If player has 1 or more Rune Points
/// Level up Vitality by 1
/// </summary>
/// <param name="_Vit"></param>
void UStatsMasterClass::IncreaseVIT(int _Vit)
{
	if (RunePoints >= 1)
	{
		++VitalityLvl;
		--RunePoints;

		Health = (VitalityLvl * 3) + Health;
	}
}


/// <Will>
/// <Level up STR>
/// If player has 1 or more Rune Points
/// Level up Strength by 1
/// </summary>
/// <param name="_Str"></param>
void UStatsMasterClass::IncreaseSTR(int _Str)
{
	if (RunePoints >= 1)
	{
		++StrenghtLvl;
		--RunePoints;

		//need clarification on damage increase
	}
}


/// <Will>
/// <Level up INT>
/// If player has 1 or more Rune Points
/// Level up Inteligence by 1
/// </summary>
/// <param name="_Int"></param>
void UStatsMasterClass::IncreaseINT(int _Int)
{
	if (RunePoints >= 1)
	{
		++IntelligenceLvl;
		--RunePoints;

		FocusPoints = (IntelligenceLvl * 2) + FocusPoints;
	}
}


/// <Will>
/// <Level up END>
/// If player has 1 or more Rune Points
/// Level up Endurence by 1
/// </summary>
/// <param name="_End"></param>
void UStatsMasterClass::IncreaseEND(int _End)
{
	if (RunePoints >= 1)
	{
		++EnduranceLvl;
		--RunePoints;

		Stamina = (EnduranceLvl * 3) + Stamina;
	}
}


//Stats Modifiers
//May not be needed, Keeping in case of change

///// <Will>
///// <Upgrading >
///// HP = Base health + X HP/LVL
///// </summary>
///// <param name="_Health"></param>
//void UStatsMasterClass::ModifyHP(float _Health)
//{
//
//}
//
///// <Will>
///// <summary>
///// 
///// </summary>
///// <param name="_Focus"></param>
//void UStatsMasterClass::ModifyFP(float _Focus)
//{
//}
//
///// <Will>
///// <summary>
///// 
///// </summary>
///// <param name="_Stamina"></param>
//void UStatsMasterClass::ModifyStamina(float _Stamina)
//{
//}
//
///// <Will>
///// <summary>
///// 
///// </summary>
///// <param name="_Damage"></param>
//void UStatsMasterClass::ModifyDamage(float _Damage)
//{
//
//}


//Runes

/// <Will>
/// <summary>
/// If the player has a LESSER rune
/// Gain one Rune Point
/// 
/// Inventory check?
/// OR
/// On use?
/// </summary>
/// <param name="_Les"></param>
void UStatsMasterClass::Lesser(int _Les)
{
	//Subject to change

	if (LesserRune >= 1)
	{
		RunePoints += _Les;
	}
}

/// <Will>
/// <summary>
/// If the player has a GREATER rune
/// Gain one Level in EVERY stats
/// 
/// Inventory check?
/// OR
/// On use?
/// </summary>
/// <param name="_Grt"></param>
void UStatsMasterClass::Greater(int _Grt)
{
	//Subject to change
	//could use bool for instant lvl up?

	if (GreaterRune >= 1)
	{
		VitalityLvl += _Grt;
		StrenghtLvl += _Grt;
		IntelligenceLvl += _Grt;
		EnduranceLvl += _Grt;
	}
}
