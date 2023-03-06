// Fill out your copyright notice in the Description page of Project Settings.

/*
* Script Name:	WeaponResetNotify.h
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/

#include "WeaponAttackNotify.h"
#include "Engine.h"

[[deprecated]] void UWeaponAttackNotify::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());

	/// NULL Check
	if (Player != nullptr && Player->EquippedWeapon != nullptr) {
		//Player->EquippedWeapon->DealDamage();
	}
}