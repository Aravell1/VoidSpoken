// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "PlayerCharacter.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "WeaponAttackNotifyState.generated.h"

/*
* Script Name:	WeaponAttackNotifyState.h
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/

UCLASS()
class VOIDSPOKEN_API UWeaponAttackNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) override;
	
};
