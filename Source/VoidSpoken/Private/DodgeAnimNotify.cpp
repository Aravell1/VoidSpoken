// Fill out your copyright notice in the Description page of Project Settings.

/*
* Script Name:	WeaponResetNotify.h
* Author:		YunYun (Jan Skucinski)
* Email:		Jan.Frank.Skucinski@gmail.com
*/

#include "DodgeAnimNotify.h"
#include "Engine.h"

void UDodgeAnimNotify::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, __FUNCTION__);
#endif

	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());

	/// NULL Check
	if (Player != nullptr) {
		Player->SetInvincibility();
		Player->SetInvincibility();
	}
}