// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAttackNotifyState.h"

void UEnemyAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration)
{
	ABaseEnemy* Enemy = Cast<ABaseEnemy>(MeshComponent->GetOwner());

	if (Enemy != nullptr && Enemy->EquippedWeapon != nullptr)
		Enemy->EquippedWeapon->SetIsAttacking(true);
}

void UEnemyAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation)
{
	ABaseEnemy* Enemy = Cast<ABaseEnemy>(MeshComponent->GetOwner());

	if (Enemy != nullptr && Enemy->EquippedWeapon != nullptr)
		Enemy->EquippedWeapon->SetIsAttacking(false);
}
