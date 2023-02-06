// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "PlayerCharacter.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "WeaponClearNotify.generated.h"

/**
 * 
 */
UCLASS()
class VOIDSPOKEN_API UWeaponClearNotify : public UAnimNotify
{
	GENERATED_BODY()

	public:
	virtual void Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation);
	
};
