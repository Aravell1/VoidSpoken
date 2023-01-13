// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsMasterClass.h"

// Sets default values for this component's properties
UStatsMasterClass::UStatsMasterClass()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


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
		SetMaxHealthPlayer();
	}
}


/// <Will>
/// <Level up STR>
/// If player has 1 or more Rune Points
/// Level up Strength by 1
/// 
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
		SetMaxFocusPlayer();
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
		SetMaxStaminaPlayer();
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
		SetMaxHealthPlayer();

		++StrenghtLvl;
		
		++IntelligenceLvl;
		SetMaxFocusPlayer();

		++EnduranceLvl;
		SetMaxStaminaPlayer();
	}
}


//Sets Max HP with player's vitality LVL and base health
//Called when MaxHealth needs to update
void UStatsMasterClass::SetMaxHealthPlayer()
{
	MaxHealth = ((VitalityLvl - 1) * 3.0f) + BaseHealth;

	//UE_LOG(LogTemp, Warning, TEXT("Max Player Health: %f"), MaxHealth);
}

void UStatsMasterClass::SetMaxHealth(float MHP)
{
	MaxHealth = MHP;
}


//Sets Max Focus with player's inteligence LVL and base Focus points
//Called when MaxHealth needs to update
void UStatsMasterClass::SetMaxFocusPlayer()
{
	MaxFocus = ((IntelligenceLvl - 1) * 3.0f) + BaseFocus;

	//UE_LOG(LogTemp, Warning, TEXT("Max Focus Points: %f"), MaxFocus);
}

void UStatsMasterClass::SetMaxFocus(float MFocus)
{
	MaxFocus = MFocus;
}


//Sets Max Stamina with player's endurance LVL and base Stamina
//Called when MaxStamina needs to update
void UStatsMasterClass::SetMaxStaminaPlayer()
{
	MaxStamina = ((EnduranceLvl - 1) * 3.0f) + BaseStamina;
	
	//UE_LOG(LogTemp, Warning, TEXT("Max Stamina: %f"), MaxStamina);
}

void UStatsMasterClass::SetMaxStamina(float MStam)
{
	MaxStamina = MStam;
}


// Initialize max stats when game starts
void UStatsMasterClass::InitializeMaxStats()
{
	SetMaxHealthPlayer();
	SetMaxFocusPlayer();
	SetMaxStaminaPlayer();
}


//Initialize Main Stats on BeginPlay to correct values
void UStatsMasterClass::InitializeMainStats()
{
	Health = MaxHealth;
	UE_LOG(LogTemp, Warning, TEXT("Player Health: %f"), Health);
	
	FocusPoints = MaxFocus;
	UE_LOG(LogTemp, Warning, TEXT("Focus Points: %f"), FocusPoints);
	
	Stamina = MaxStamina;
	UE_LOG(LogTemp, Warning, TEXT("Max Stamina: %f"), Stamina);
}
