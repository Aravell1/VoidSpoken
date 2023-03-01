// Fill out your copyright notice in the Description page of Project Settings.


#include "StaminaPickup.h"

AStaminaPickup::AStaminaPickup()
{
	TextTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Text Trigger Box"));
	TextTriggerBox->SetupAttachment(RootComponent);
}

void AStaminaPickup::BeginPlay()
{
	Super::BeginPlay();
}


void AStaminaPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherComp)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

		if (Player && Player->GetMesh())
		{
			Player->SetOverlappingItem(this);
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