// Fill out your copyright notice in the Description page of Project Settings.

/*
* Script Name:	WeaponAttackNotifyState.h
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/

#include "WeaponUniqueAttackNotifyState.h"
#include "Engine.h"

void UWeaponUniqueAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration) {
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, __FUNCTION__);
#endif

	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());

	/// NULL Check
	if (Player != nullptr && Player->EquippedWeapon != nullptr) Player->EquippedWeapon->SetCanUniqueAttack(true);
}

[[deprecated]] void UWeaponUniqueAttackNotifyState::NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime) {
#if WITH_EDITOR
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, __FUNCTION__);
#endif
}

void UWeaponUniqueAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, __FUNCTION__);
#endif

	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());

	/// NULL Check
	if (Player != nullptr && Player->EquippedWeapon != nullptr) Player->EquippedWeapon->SetCanUniqueAttack(false);
}