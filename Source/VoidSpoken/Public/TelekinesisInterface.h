// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TelekinesisInterface.generated.h"

UENUM(BlueprintType)
enum class ETelekinesisState : uint8 {
	ETS_Default = 0 UMETA(DisplayName = "Default"),
	ETS_Pulled = 1 UMETA(DisplayName = "Pulled"),
	ETS_Pushed = 2 UMETA(DisplayName = "Pushed")
};

/**
 * 
 */
UINTERFACE(MinimalAPI, BlueprintType)
class UTelekinesisInterface : public UInterface
{
	GENERATED_BODY()	
};

class VOIDSPOKEN_API ITelekinesisInterface {
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category = "Telekinesis")
		void Highlight(bool bHighlight);

	UFUNCTION(BlueprintNativeEvent, Category = "Telekinesis")
		void Push(FVector Destination, float Force);

	UFUNCTION(BlueprintNativeEvent, Category = "Telekinesis")
		void Pull(ACharacter* Character);

	UFUNCTION(BlueprintNativeEvent, Category = "Telekinesis")
		void Drop();

};
