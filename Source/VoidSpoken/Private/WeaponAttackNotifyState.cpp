// Fill out your copyright notice in the Description page of Project Settings.

/*
* Script Name:	WeaponAttackNotifyState.cpp
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/

#include "WeaponAttackNotifyState.h"
#include "Engine.h"

void UWeaponAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration) {
	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());

	/// NULL Check
	if (Player != nullptr && Player->EquippedWeapon != nullptr) Player->EquippedWeapon->SetAttackDelay(true);
}

[[deprecated]] void UWeaponAttackNotifyState::NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime) {
	
}

void UWeaponAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());

	/// NULL Check
	if (Player != nullptr && Player->EquippedWeapon != nullptr) {
		Player->EquippedWeapon->SetAttackDelay(false);
		Player->EquippedWeapon->NextAttack();
	}
}