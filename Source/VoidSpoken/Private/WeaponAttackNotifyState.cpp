// Fill out your copyright notice in the Description page of Project Settings.

/*
* Script Name:	WeaponAttackNotifyState.cpp
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/

#include "WeaponAttackNotifyState.h"
#include "Engine.h"

void UWeaponAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration) {
	const APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());
	if (Player != nullptr && Player->EquippedWeapon != nullptr)
		Player->EquippedWeapon->SetCheckForOverlappedActors(true);
}

void UWeaponAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
	const APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());
	if (Player != nullptr && Player->EquippedWeapon != nullptr)
		Player->EquippedWeapon->SetCheckForOverlappedActors(false);
}