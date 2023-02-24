// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPickup.h"

AHealthPickup::AHealthPickup()
{
	TextTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Text Trigger Box"));
	TextTriggerBox->SetupAttachment(RootComponent);
}

void AHealthPickup::BeginPlay()
{
	Super::BeginPlay();

	TextTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AHealthPickup::TextTriggerOverlapBegin);
	TextTriggerBox->OnComponentEndOverlap.AddDynamic(this, &AHealthPickup::TextTriggerOverlapEnd);
}

void AHealthPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

		if (Player)
		{
			Player->SetOverlappingItem(this);
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

void AHealthPickup::TextTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void AHealthPickup::TextTriggerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}
