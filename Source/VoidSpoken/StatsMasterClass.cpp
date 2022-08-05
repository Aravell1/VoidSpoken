// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsMasterClass.h"

// Sets default values for this component's properties
UStatsMasterClass::UStatsMasterClass()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	Health = 30.0f;
	MaxHealth;

	FocusPoints = 20.0f;
	MaxFocus;

	Stamina = 50.0f;
	MaxStamina;

	Damage;

	VitalityLvl = 1.0f;
	StrenghtLvl = 1.0f;
	IntelligenceLvl = 1.0f;
	EnduranceLvl = 1.0f;
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
void UStatsMasterClass::RegenHP(int _HP)
{

}

/// <NAME>
/// <Focus Regeneration?>
/// ???
/// </summary>
/// <param name="_FP"></param>
void UStatsMasterClass::RegenFP(int _FP)
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
void UStatsMasterClass::RegenStamina(int _ST)
{

}



//Leveling

/// <Will>
/// <Level up VIT>
/// If player has 1 or more Rune Points
/// Level up Vitality by 1
/// </summary>
/// <param name="_Vit"></param>
void UStatsMasterClass::IncreaseVIT()
{
	if (RunePoints >= 1)
	{
		++VitalityLvl;
		--RunePoints;
		UE_LOG(LogTemp, Warning, TEXT("VITALITY level is now %f"), VitalityLvl);

		// Increase Player HP
		MaxHealth = (VitalityLvl * 3.0f) + Health;
		UE_LOG(LogTemp, Warning, TEXT("Max Player Health: %f"), MaxHealth);
	}
}


/// <Will>
/// <Level up STR>
/// If player has 1 or more Rune Points
/// Level up Strength by 1
/// </summary>
/// <param name="_Str"></param>
void UStatsMasterClass::IncreaseSTR()
{
	if (RunePoints >= 1)
	{
		++StrenghtLvl;
		--RunePoints;
		UE_LOG(LogTemp, Warning, TEXT("STRENGTH level is now %f"), StrenghtLvl);

		//need clarification on damage increase
		//UE_LOG(LogTemp, Warning, TEXT("Player base Damage: %d"), Damage);
	}
}


/// <Will>
/// <Level up INT>
/// If player has 1 or more Rune Points
/// Level up Inteligence by 1
/// </summary>
/// <param name="_Int"></param>
void UStatsMasterClass::IncreaseINT()
{
	if (RunePoints >= 1)
	{
		++IntelligenceLvl;
		--RunePoints;
		UE_LOG(LogTemp, Warning, TEXT("INTELLIGENCE level is now %f"), IntelligenceLvl);

		// Increase Player Focus Points
		MaxFocus = (IntelligenceLvl * 2.0f) + FocusPoints;
		UE_LOG(LogTemp, Warning, TEXT("Focus Points: %d"), FocusPoints);
	}
}


/// <Will>
/// <Level up END>
/// If player has 1 or more Rune Points
/// Level up Endurence by 1
/// </summary>
/// <param name="_End"></param>
void UStatsMasterClass::IncreaseEND()
{
	if (RunePoints >= 1)
	{
		++EnduranceLvl;
		--RunePoints;
		UE_LOG(LogTemp, Warning, TEXT("ENDURANCE level is now %f"), EnduranceLvl);
		

		// Increase Player Stamina
		MaxStamina = (EnduranceLvl * 3.0f) + Stamina;
		UE_LOG(LogTemp, Warning, TEXT("Player Stamina: %f"), FocusPoints);
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
void UStatsMasterClass::Lesser()
{
	//Subject to change

	if (LesserRune >= 1)
	{
		++RunePoints;
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
void UStatsMasterClass::Greater()
{
	//Subject to change
	//could use bool for instant lvl up?

	if (GreaterRune >= 1)
	{
		++VitalityLvl;
		++StrenghtLvl;
		++IntelligenceLvl;
		++EnduranceLvl;
	}
}
