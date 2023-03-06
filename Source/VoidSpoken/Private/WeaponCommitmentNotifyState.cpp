// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponCommitmentNotifyState.h"
#include "PlayerCharacter.h"

void UWeaponCommitmentNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration) {
	const APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());
	if (Player != nullptr && Player->EquippedWeapon != nullptr)
		Player->EquippedWeapon->SetAttackDelay(true);
}

void UWeaponCommitmentNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
	const APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());
	if (Player != nullptr && Player->EquippedWeapon != nullptr)
		Player->EquippedWeapon->SetAttackDelay(false);
}

