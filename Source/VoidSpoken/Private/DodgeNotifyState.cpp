// Fill out your copyright notice in the Description page of Project Settings.


#include "DodgeNotifyState.h"

void UDodgeNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration) {
	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());
	if (Player)
		Player->DodgingStarted();
}

void UDodgeNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());
	if (Player)
		Player->DodgingFinished();
}
