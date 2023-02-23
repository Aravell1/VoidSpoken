// Fill out your copyright notice in the Description page of Project Settings.


#include "FocusPickup.h"

AFocusPickup::AFocusPickup()
{
	TextTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Text Trigger Box"));
	TextTriggerBox->SetupAttachment(RootComponent);
}

void AFocusPickup::BeginPlay()
{
	Super::BeginPlay();

	TextTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFocusPickup::TextTriggerOverlapBegin);
	TextTriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFocusPickup::TextTriggerOverlapEnd);
}

void AFocusPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void AFocusPickup::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

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

void AFocusPickup::TextTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void AFocusPickup::TextTriggerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

