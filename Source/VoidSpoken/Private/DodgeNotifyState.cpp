// Fill out your copyright notice in the Description page of Project Settings.


#include "DodgeNotifyState.h"

void UDodgeNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float TotalDuration) {
	#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, __FUNCTION__);
	#endif

	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());

	if (Player)
		Player->DodgingStarted();
}

[[deprecated]] void UDodgeNotifyState::NotifyTick(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation, float FrameDeltaTime) {

}

void UDodgeNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComponent, UAnimSequenceBase* Animation) {
	#if WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, __FUNCTION__);
	#endif

	APlayerCharacter* Player = Cast<APlayerCharacter>(MeshComponent->GetOwner());

	if (Player)
		Player->DodgingFinished();
}
