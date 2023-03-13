// Fill out your copyright notice in the Description page of Project Settings.


#include "VoidSpokenGameInstance.h"
#include "Kismet/GameplayStatics.h"

//#include "PlayerSaveGame.h"
//#include "ObeliskSaveGame.h"

bool UVoidSpokenGameInstance::DoesSaveGameExist(FString SlotName)
{
	return UGameplayStatics::DoesSaveGameExist(SlotName, 0);
}

void UVoidSpokenGameInstance::Init()
{
	Super::Init();

	// Check if the save game exists
	if (DoesSaveGameExist(PlayerSlotName))
	{
		// Load the existing save game
		PlayerSaveGame = Cast<UMasterSaveGame>(UGameplayStatics::LoadGameFromSlot(PlayerSlotName, 0));
	}
	else
	{
		// Create a new save game
		PlayerSaveGame = Cast<UMasterSaveGame>(UGameplayStatics::CreateSaveGameObject(UMasterSaveGame::StaticClass()));

		// Save the new save game
		//UGameplayStatics::SaveGameToSlot(PlayerSaveGame, PlayerSlotName, 0);
	}
}
void UVoidSpokenGameInstance::ResetSaveGameData()
{
	// Reset the player save game data
	//PlayerSaveGame = NewObject<UMasterSaveGame>();
	//PlayerSaveGame->Reset();

	// Reset the inventory save game data
	AVoidSpokenGameModeBase* GameMode = Cast<AVoidSpokenGameModeBase>(GetWorld()->GetAuthGameMode());
	GameMode->SetHealItem(0);
	GameMode->SetFocusItem(0);
	GameMode->SetStaminaItem(0);
}

void UVoidSpokenGameInstance::LoadInventorySaveGame()
{
	// Load the save game and update the game mode item pickup values
	PlayerSaveGame = Cast<UMasterSaveGame>(UGameplayStatics::LoadGameFromSlot(PlayerSlotName, 0));
	if (PlayerSaveGame != nullptr)
	{
		AVoidSpokenGameModeBase* GameMode = Cast<AVoidSpokenGameModeBase>(GetWorld()->GetAuthGameMode());
		if (GameMode != nullptr)
		{
			// Add the loaded item pickups to the existing values in the game mode
			GameMode->SetHealItem(PlayerSaveGame->HealPickup);
			GameMode->SetFocusItem(PlayerSaveGame->FocusPickup);
			GameMode->SetStaminaItem(PlayerSaveGame->StaminaPickup);

			// Log a message for each item loaded
			if (PlayerSaveGame->HealPickup > 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Heal pickup loaded."));
			}
			if (PlayerSaveGame->FocusPickup > 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Focus pickup loaded."));
			}
			if (PlayerSaveGame->StaminaPickup > 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("Stamina pickup loaded."));
			}
		}
	}

	//AVoidSpokenGameModeBase* GameMode = Cast<AVoidSpokenGameModeBase>(GetWorld()->GetAuthGameMode());
	//// Load the save game and update the game mode item pickup values
	//PlayerSaveGame = Cast<UMasterSaveGame>(UGameplayStatics::LoadGameFromSlot(PlayerSlotName, 0));
	//if (PlayerSaveGame != nullptr)
	//{
	//	if (GameMode != nullptr)
	//	{
	//		GameMode->SetHealItem(PlayerSaveGame->HealPickup);
	//		GameMode->SetFocusItem(PlayerSaveGame->FocusPickup);
	//		GameMode->SetStaminaItem(PlayerSaveGame->StaminaPickup);

	//		// Log a message for each item loaded
	//		if (PlayerSaveGame->HealPickup)
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("Heal pickup loaded."));
	//		}
	//		if (PlayerSaveGame->FocusPickup)
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("Focus pickup loaded."));
	//		}
	//		if (PlayerSaveGame->StaminaPickup)
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("Stamina pickup loaded."));
	//		}
	//	}
	//}
}

void UVoidSpokenGameInstance::SaveInventorySaveGame()
{
	// Get a reference to the game mode and save the item pickup values
	AVoidSpokenGameModeBase* GameMode = Cast<AVoidSpokenGameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode != nullptr && PlayerSaveGame != nullptr)
	{
		if (GameMode->GetHealItem() > PlayerSaveGame->HealPickup)
		{
			PlayerSaveGame->HealPickup = GameMode->GetHealItem();
		}
		if (GameMode->GetFocusItem() > PlayerSaveGame->FocusPickup)
		{
			PlayerSaveGame->FocusPickup = GameMode->GetFocusItem();
		}
		if (GameMode->GetStaminaItem() > PlayerSaveGame->StaminaPickup)
		{
			PlayerSaveGame->StaminaPickup = GameMode->GetStaminaItem();
		}

		//PlayerSaveGame->HealPickup = GameMode->GetHealItem();
		//PlayerSaveGame->FocusPickup = GameMode->GetFocusItem();
		//PlayerSaveGame->StaminaPickup = GameMode->GetStaminaItem();
		UGameplayStatics::SaveGameToSlot(PlayerSaveGame, PlayerSlotName, 0);

		// Log a message for each item saved
		if (PlayerSaveGame->HealPickup)
		{
			UE_LOG(LogTemp, Warning, TEXT("Heal pickup saved."));
		}
		if (PlayerSaveGame->FocusPickup)
		{
			UE_LOG(LogTemp, Warning, TEXT("Focus pickup saved."));
		}
		if (PlayerSaveGame->StaminaPickup)
		{
			UE_LOG(LogTemp, Warning, TEXT("Stamina pickup saved."));
		}
	}
}

void UVoidSpokenGameInstance::LoadPlayerSaveGame()
{
	if (DoesSaveGameExist(PlayerSlotName))
	{
		PlayerSaveGame = Cast<UMasterSaveGame>(UGameplayStatics::LoadGameFromSlot(PlayerSlotName, 0));
		if (PlayerSaveGame)
		{
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController)
			{
				APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetCharacter());
				if (PlayerCharacter)
				{
					FVector PlayerLocation = PlayerSaveGame->PlayerLocation;
					FRotator PlayerRotation = PlayerSaveGame->PlayerRotation;

					// Teleport the player to the saved location
					PlayerCharacter->SetActorLocation(PlayerLocation);
					PlayerCharacter->SetActorRotation(PlayerRotation);

					UE_LOG(LogTemp, Warning, TEXT("Player location loaded from save game: %s"), *PlayerLocation.ToString());
				}
			}
		}
	}
}

void UVoidSpokenGameInstance::SavePlayerSaveGame()
{
	ABaseEntity* PlayerEntity = nullptr;
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetCharacter());
		if (PlayerCharacter)
		{
			PlayerEntity = Cast<ABaseEntity>(PlayerCharacter);
			if (PlayerEntity)
			{
				// Save the player's transform
				FVector PlayerLocation = PlayerEntity->GetActorLocation();
				FRotator PlayerRotation = PlayerEntity->GetActorRotation();

				UE_LOG(LogTemp, Warning, TEXT("Saving player transform to slot %s"), *PlayerSlotName);
				UE_LOG(LogTemp, Warning, TEXT("Player location: %s"), *PlayerLocation.ToString());
				UE_LOG(LogTemp, Warning, TEXT("Player rotation: %s"), *PlayerRotation.ToString());

				// Save the transform to the save game object
				PlayerSaveGame->PlayerLocation = PlayerLocation;
				PlayerSaveGame->PlayerRotation = PlayerRotation;

				// Save the save game object
				UGameplayStatics::SaveGameToSlot(PlayerSaveGame, PlayerSlotName, 0);
			}
		}
	}
}

TArray<AObelisk*> UVoidSpokenGameInstance::GetAllObelisks()
{
	TArray<AObelisk*> Obelisks;
	for (TActorIterator<AObelisk> ObeliskItr(GetWorld()); ObeliskItr; ++ObeliskItr)
	{
		Obelisks.Add(*ObeliskItr);
	}
	return Obelisks;
}


void UVoidSpokenGameInstance::LoadObeliskSaveGame()
{
	// Load the existing save game or create a new one if it doesn't exist
	if (DoesSaveGameExist(PlayerSlotName))
	{
		PlayerSaveGame = Cast<UMasterSaveGame>(UGameplayStatics::LoadGameFromSlot(PlayerSlotName, 0));
	}
	else
	{
		PlayerSaveGame = Cast<UMasterSaveGame>(UGameplayStatics::CreateSaveGameObject(UMasterSaveGame::StaticClass()));
	}

	// Get all the obelisks in the game
	TArray<AObelisk*> Obelisks = GetAllObelisks();

	for (AObelisk* Obelisk : Obelisks)
	{
		FString ObeliskName = Obelisk->GetObeliskName();
		int32 Index = PlayerSaveGame->ObeliskSaveData.IndexOfByPredicate([&ObeliskName](const FObeliskSaveData& ObeliskData)
		{
			return ObeliskData.ObeliskName == ObeliskName;
		});

		if (Index != INDEX_NONE)
		{
			EActivationState ObeliskState = PlayerSaveGame->ObeliskSaveData[Index].ObeliskState;
			Obelisk->SetObeliskState(ObeliskState);

			UE_LOG(LogTemp, Warning, TEXT("Loaded obelisk name: %s, state: %d"), *ObeliskName, static_cast<int32>(ObeliskState));
		}
	}
}

void UVoidSpokenGameInstance::SaveObeliskSaveGame()
{
	// Get all the obelisks in the game
	TArray<AObelisk*> Obelisks = GetAllObelisks();

	if (DoesSaveGameExist(PlayerSlotName))
	{
		PlayerSaveGame = Cast<UMasterSaveGame>(UGameplayStatics::LoadGameFromSlot(PlayerSlotName, 0));
	}
	else
	{
		PlayerSaveGame = Cast<UMasterSaveGame>(UGameplayStatics::CreateSaveGameObject(UMasterSaveGame::StaticClass()));
	}

	// Loop through each obelisk and update the save game with its name and state
	for (AObelisk* Obelisk : Obelisks)
	{
		FString ObeliskName = Obelisk->GetObeliskName();
		EActivationState ObeliskState = Obelisk->GetObeliskState();
		int32 Index = PlayerSaveGame->ObeliskSaveData.IndexOfByPredicate([&ObeliskName](const FObeliskSaveData& ObeliskData)
		{
			return ObeliskData.ObeliskName == ObeliskName;
		});

		// If the entry doesn't exist, add a new one
		if (Index == INDEX_NONE)
		{
			FObeliskSaveData NewObeliskData;
			NewObeliskData.ObeliskName = ObeliskName;
			NewObeliskData.ObeliskState = ObeliskState;
			PlayerSaveGame->ObeliskSaveData.Add(NewObeliskData);
		}
		else
		{
			PlayerSaveGame->ObeliskSaveData[Index].ObeliskState = ObeliskState;
		}
		UE_LOG(LogTemp, Warning, TEXT("Saved obelisk name: %s, state: %d"), *ObeliskName, static_cast<int32>(ObeliskState));
	}
	UGameplayStatics::SaveGameToSlot(PlayerSaveGame, PlayerSlotName, 0);
}
