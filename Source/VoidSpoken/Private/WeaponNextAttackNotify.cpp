// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponNextAttackNotify.h"
#include "PlayerCharacter.h"

void UWeaponNextAttackNotify::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());

	/// NULL Check
	if (Player != nullptr && Player->EquippedWeapon != nullptr)
		Player->EquippedWeapon->NextAttack();

	#if WITH_ENGINE
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, __FUNCTION__);
	#endif
}