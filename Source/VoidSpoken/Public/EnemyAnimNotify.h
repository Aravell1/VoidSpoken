// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BaseEnemy.h"
#include "EnemyAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class VOIDSPOKEN_API UEnemyAnimNotify : public UAnimNotify
{
	GENERATED_BODY()
	
protected:

	virtual void Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) override;

};
