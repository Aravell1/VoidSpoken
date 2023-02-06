// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BaseEnemy.h"
#include "Ghoul.h"
#include "Gatekeeper.h"
#include "EnemyAttackNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class VOIDSPOKEN_API UEnemyAttackNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	
protected:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) override;
};
