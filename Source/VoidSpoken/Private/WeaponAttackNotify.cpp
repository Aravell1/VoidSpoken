// Fill out your copyright notice in the Description page of Project Settings.

/*
* Script Name:	WeaponResetNotify.h
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/

#include "WeaponAttackNotify.h"
#include "Engine.h"

void UWeaponAttackNotify::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, __FUNCTION__);
#endif
	
	APlayerCharacter* PlayerReference = Cast<APlayerCharacter>(MeshComponent->GetOwner());

	/// NULL Check
	if (PlayerReference != nullptr && PlayerReference->EquippedWeapon != nullptr) {
		PlayerReference->EquippedWeapon->DealDamage();
	}

}