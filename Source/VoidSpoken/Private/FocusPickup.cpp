// Fill out your copyright notice in the Description page of Project Settings.


#include "FocusPickup.h"

AFocusPickup::AFocusPickup()
{
	
}

void AFocusPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

		if (Player)
		{
			Player->SetOverlappingItem(this);
			//UE_LOG(LogTemp, Warning, TEXT("Focus Overlap"));
		}
	}
}

void AFocusPickup::PickupFocus()
{
	AVoidSpokenGameModeBase* GM;
	GM = Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
	
	if (GM->GetFocusItem() < GM->MaxFocus)
	{
		GM->SetFocusItem(1);
		Destroy();
	}
	else
		GM->PickupFull();
}

