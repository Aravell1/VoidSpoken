// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponClearNotify.h"
#include "Engine.h"

void UWeaponClearNotify::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());

	/// NULL Check
	if (Player != nullptr && Player->EquippedWeapon != nullptr) Player->EquippedWeapon->Clear();
}