// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Public/BaseWeapon.h"
#include "../Public/BaseEntity.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "WeaponUniqueAttackNotifyState.generated.h"

/*
* Script Name:	WeaponAttackNotifyState.h
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/

UCLASS()
class VOIDSPOKEN_API UWeaponUniqueAttackNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) override;
};
