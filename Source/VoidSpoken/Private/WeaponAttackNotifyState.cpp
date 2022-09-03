// Fill out your copyright notice in the Description page of Project Settings.

/*
* Script Name:	WeaponAttackNotifyState.cpp
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/

#include "WeaponAttackNotifyState.h"
#include "Engine.h"

void UWeaponAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration) {
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, __FUNCTION__);
#endif

	APlayerCharacter* PlayerReference = Cast<APlayerCharacter>(MeshComponent->GetOwner());

	/// NULL Check
	if (PlayerReference != nullptr && PlayerReference->EquippedWeapon != nullptr) PlayerReference->EquippedWeapon->SetAttackDelay(true);
}

[[deprecated]] void UWeaponAttackNotifyState::NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime) {
#if WITH_EDITOR
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, __FUNCTION__);
#endif
}

void UWeaponAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, __FUNCTION__);
#endif

	APlayerCharacter* PlayerReference = Cast<APlayerCharacter>(MeshComponent->GetOwner());

	/// NULL Check
	if (PlayerReference != nullptr && PlayerReference->EquippedWeapon != nullptr) {
		PlayerReference->EquippedWeapon->SetAttackDelay(false);
		PlayerReference->EquippedWeapon->NextAttack();
	}
}