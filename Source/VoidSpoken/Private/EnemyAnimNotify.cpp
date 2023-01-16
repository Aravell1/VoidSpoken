// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyAnimNotify.h"

void UEnemyAnimNotify::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation)
{
	ABaseEnemy* Enemy = Cast<ABaseEnemy>(MeshComponent->GetOwner());

	if (Enemy != nullptr)
	{
		if (Cast<AGhoul>(Enemy))
		{
			Cast<AGhoul>(Enemy)->TriggerSpikes(Cast<UAnimMontage>(Animation));
		}
		else
		{
			Enemy->AttackTrace(Cast<UAnimMontage>(Animation));
		}
	}
}
