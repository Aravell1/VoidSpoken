// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../PlayerCharacter.h"
#include "../Public/BaseWeapon.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "WeaponResetNotify.generated.h"

/*
* Script Name:	WeaponResetNotify.h
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/
UCLASS()
class VOIDSPOKEN_API UWeaponResetNotify : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) override;
};
