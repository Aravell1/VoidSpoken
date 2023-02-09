// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "WeaponNextAttackNotify.generated.h"

/**
 * 
 */
UCLASS()
class VOIDSPOKEN_API UWeaponNextAttackNotify : public UAnimNotify
{
	GENERATED_BODY()

	void Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation);
};
