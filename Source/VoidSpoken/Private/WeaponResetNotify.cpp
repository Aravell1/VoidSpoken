// Fill out your copyright notice in the Description page of Project Settings.

/*
* Script Name:	WeaponResetNotify.cpp
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/

#include "WeaponResetNotify.h"
#include "PlayerCharacter.h"

void UWeaponResetNotify::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
	const APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());
	/// NULL Check
	if (Player != nullptr && Player->EquippedWeapon != nullptr)
		Player->EquippedWeapon->Reset();
}
