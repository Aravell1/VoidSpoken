// Fill out your copyright notice in the Description page of Project Settings.


#include "DodgingResetNotify.h"
#include "PlayerCharacter.h"

void UDodgingResetNotify::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());
	if (Player)
		Player->ResetDodging();
}
