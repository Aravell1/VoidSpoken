// Fill out your copyright notice in the Description page of Project Settings.


#include "FocusPickup.h"

AFocusPickup::AFocusPickup()
{
	
}

void AFocusPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this && OtherActor->IsA<APlayerCharacter>())
	{
		//Focus Pickup++
		AVoidSpokenGameModeBase* GM;
		GM = Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
		if (GM->GetFocusItem() < GM->MaxFocus)
		{
			GM->SetFocusItem(1);
			Destroy();
		}
		GM->PickupFull();
	}
	else
		return;
}
