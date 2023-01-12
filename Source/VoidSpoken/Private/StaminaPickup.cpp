// Fill out your copyright notice in the Description page of Project Settings.


#include "StaminaPickup.h"

AStaminaPickup::AStaminaPickup()
{
}


void AStaminaPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this && OtherActor->IsA<APlayerCharacter>())
	{
		//Focus Pickup++
		AVoidSpokenGameModeBase* GM;

		GM = Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GM)
		{
			GM->SetStaminaItem(1);
		}
		Destroy();
	}
	else
		return;
}
