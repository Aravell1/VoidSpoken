// Fill out your copyright notice in the Description page of Project Settings.

/*
* Script Name:	WeaponResetNotify.cpp
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/

#include "WeaponResetNotify.h"
#include "Engine.h"

void UWeaponResetNotify::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, __FUNCTION__);
#endif

	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());

	/// NULL Check
	if (Player != nullptr && Player->EquippedWeapon != nullptr) Player->EquippedWeapon->Reset();
}
