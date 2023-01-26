// Fill out your copyright notice in the Description page of Project Settings.


#include "StaminaPickup.h"

AStaminaPickup::AStaminaPickup()
{
	
}


void AStaminaPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
		//Player->PlayerCapsule = Cast<UCapsuleComponent>(OverlappedComponent);
		//Player->PlayerMesh = Cast<USkeletalMeshComponent>(OverlappedComponent);

		if (Player)
		{
			Player->SetOverlappingItem(this);
			//UE_LOG(LogTemp, Warning, TEXT("Stamina Overlap"));
			// Pickup Message
		}
		else
			return;
	}
}

void AStaminaPickup::PickupStamina()
{
	AVoidSpokenGameModeBase* GM;
	GM = Cast<AVoidSpokenGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));

	if (GM->GetStaminaItem() < GM->MaxStamina)
	{
		GM->SetStaminaItem(1);
		Destroy();
	}
	else
		GM->PickupFull();
}
