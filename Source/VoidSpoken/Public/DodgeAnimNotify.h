// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "PlayerCharacter.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "DodgeAnimNotify.generated.h"

/*
* Script Name:	DodgeAnimNotify.h
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/

UCLASS()
class VOIDSPOKEN_API UDodgeAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) override;
};
