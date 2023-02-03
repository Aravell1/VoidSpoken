// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"

AHealthPickup::AHealthPickup()
{

}

void AHealthPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

		if (Player)
		{
			Player->SetOverlappingItem(this);
			//UE_LOG(LogTemp, Warning, TEXT("Health Overlap"));
		}
	}
}

void AHealthPickup::PickupHealth()
{
	AVoidSpokenGameModeBase* GM;

	GM = Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GM->GetHealItem() < GM->MaxHeal)
	{
		GM->SetHealItem(1);
		Destroy();
	}
	else
		GM->PickupFull();
}
