// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "WeaponCommitmentNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class VOIDSPOKEN_API UWeaponCommitmentNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

	void NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration);
	void NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation);
};
