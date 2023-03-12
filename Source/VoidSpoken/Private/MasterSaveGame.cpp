// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterSaveGame.h"

void UMasterSaveGame::Reset()
{
	PlayerLocation = FVector::ZeroVector;
	PlayerRotation = FRotator::ZeroRotator;
	//ObeliskState = EActivationState::Inactive;

	UE_LOG(LogTemp, Warning, TEXT("UMasterSaveGame::Reset - Player location: %s"), *PlayerLocation.ToString());
}

void UMasterSaveGame::UpdatePlayerLocation(const FVector& NewLocation)
{
	PlayerLocation = NewLocation;

	UE_LOG(LogTemp, Warning, TEXT("UMasterSaveGame::UpdatePlayerLocation - Player location: %s"), *PlayerLocation.ToString());
}