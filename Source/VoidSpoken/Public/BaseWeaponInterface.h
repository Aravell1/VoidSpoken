// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BaseWeaponInterface.generated.h"

UCLASS()
class UWeaponSockets : public UObject {

	GENERATED_BODY()

public:

	static const FName LeftHand;
	static const FName RightHand;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBaseWeaponInterface : public UInterface
{
	GENERATED_BODY()
};


/*
* Script Name:	BaseWeaponInterface.h
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/

class VOIDSPOKEN_API IBaseWeaponInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Base Weapon")
		void Equip(ACharacter* EquippingCharacter);

};
