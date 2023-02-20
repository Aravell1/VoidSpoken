// Fill out your copyright notice in the Description page of Project Settings.


#include "DodgingResetNotify.h"
#include "PlayerCharacter.h"

void UDodgingResetNotify::Notify(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
	#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, __FUNCTION__);
	#endif

	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());
	if (Player)
		Player->ResetDodging();
}
