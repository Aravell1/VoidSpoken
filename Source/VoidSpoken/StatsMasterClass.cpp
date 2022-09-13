// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsMasterClass.h"

// Sets default values for this component's properties
UStatsMasterClass::UStatsMasterClass()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// Set Base Player Stats
	Health = 30.0f;
	MaxHealth = 0.0f;
	
	FocusPoints = 20.0f;
	MaxFocus = 0.0f;

	Stamina = 50.0f;
	MaxStamina = 0.0f;

	// Set player levels
	VitalityLvl = 1.0f;
	StrenghtLvl = 1.0f;
	IntelligenceLvl = 1.0f;
	EnduranceLvl = 1.0f;

	// Set runes values
	RunePoints = 0;
	LesserRune = 0;
	GreaterRune = 0;

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

/// <WILL>
/// <Health Regeneration>
/// if player is NOT in combat
/// Regen 2HP each 5 second
/// 
/// <param name="_HP"></param>
void UStatsMasterClass::RegenHP(int _HP)
{

}

/// <WILL>
/// <Focus Regeneration?>
/// Focus point are fully regenerated after 3 seconds
/// if the player does not use a void skill
/// 
/// <param name="_FP"></param>
void UStatsMasterClass::RegenFP(int _FP)
{

}

/// <WILL>
/// <Stamina regeneration>
/// if player IS in combat
/// Regen 2 ST each 5 second
/// 
/// OR
/// 
/// if player is NOT in combat
/// Regen 5 ST each 5 second
/// 
/// <param name="_ST"></param>
void UStatsMasterClass::RegenStamina(int _ST)
{

}



//Leveling

/// <Will>
/// <Level up VIT>
/// If player has 1 or more Rune Points
/// Level up Vitality by 1
/// 
/// <param name="_Vit"></param>
void UStatsMasterClass::IncreaseVIT()
{
	if (RunePoints >= 1)
	{
		++VitalityLvl;
		--RunePoints;
		UE_LOG(LogTemp, Warning, TEXT("VITALITY level is now %f"), VitalityLvl);

		// Increase Player HP when leveling
		SetMaxHP();
	}
}


/// <Will>
/// <Level up STR>
/// If player has 1 or more Rune Points
/// Level up Strength by 1
/// 
/// <param name="_Str"></param>
void UStatsMasterClass::IncreaseSTR()
{
	if (RunePoints >= 1)
	{
		++StrenghtLvl;
		--RunePoints;
		UE_LOG(LogTemp, Warning, TEXT("STRENGTH level is now %f"), StrenghtLvl);
	}
}


/// <Will>
/// <Level up INT>
/// If player has 1 or more Rune Points
/// Level up Inteligence by 1
/// 
/// <param name="_Int"></param>
void UStatsMasterClass::IncreaseINT()
{
	if (RunePoints >= 1)
	{
		++IntelligenceLvl;
		--RunePoints;
		UE_LOG(LogTemp, Warning, TEXT("INTELLIGENCE level is now %f"), IntelligenceLvl);

		// Increase Player Focus Points when leveling
		SetMaxFocus();
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
		

		// Increase Player Stamina when leveling
		SetMaxStamina();
	}
}



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
		SetMaxHP();

		++StrenghtLvl;
		
		++IntelligenceLvl;
		SetMaxFocus();

		++EnduranceLvl;
		SetMaxStamina();
	}
}


//Sets Max HP with player's vitality LVL and base health
//Called when MaxHealth needs to update
void UStatsMasterClass::SetMaxHP()
{
	MaxHealth = (VitalityLvl * 3.0f) + Health;
	UE_LOG(LogTemp, Warning, TEXT("Max Player Health: %f"), MaxHealth);
}


//Sets Max Focus with player's inteligence LVL and base Focus points
//Called when MaxHealth needs to update
void UStatsMasterClass::SetMaxFocus()
{
	MaxFocus = (IntelligenceLvl * 2.0f) + FocusPoints;
	UE_LOG(LogTemp, Warning, TEXT("Focus Points: %f"), FocusPoints);
}


//Sets Max Stamina with player's endurance LVL and base Stamina
//Called when MaxStamina needs to update
void UStatsMasterClass::SetMaxStamina()
{
	MaxStamina = (EnduranceLvl * 3.0f) + Stamina;
	UE_LOG(LogTemp, Warning, TEXT("Player Stamina: %f"), FocusPoints);
}

// Initialize max stats when game starts
void UStatsMasterClass::InitializeMaxStats()
{
	SetMaxHP();
	SetMaxFocus();
	SetMaxStamina();
}
