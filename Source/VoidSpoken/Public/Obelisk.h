// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "PlayerCharacter.h"
#include "BaseEnemy.h"
#include "../VoidSpokenGameModeBase.h"
#include "EnemyPortalSpawn.h"
#include "Obelisk.generated.h"

UENUM()
enum EActivationState
{
	Inactive	UMETA(DisplayName = "Inactive"),
	Charging	UMETA(DisplayName = "Charging"),
	Activated	UMETA(DisplayName = "Activated")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChargeDelegate);
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActivationDelegate, AObelisk*, Obelisk);

UCLASS()
class VOIDSPOKEN_API AObelisk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObelisk();

	UFUNCTION(BlueprintPure)
		EActivationState GetObeliskState() { return ObeliskState; }

	UFUNCTION(BlueprintCallable)
		void SetObeliskState(EActivationState NewState);

	UFUNCTION(BlueprintPure)
		float GetTotalRequiredCharge() { return TotalRequiredCharge; }

	UFUNCTION(BlueprintPure)
		float GetObeliskCharge() { return ObeliskCharge; }

	void SetCanBeginCharging(bool CanCharge) { bCanBeginCharging = CanCharge; }

	UFUNCTION()
		void AddCharge(ABaseEnemy* EnemyTrigger);

	UPROPERTY(EditAnywhere)
		TArray<AEnemyPortalSpawn*> NearbySpawnPoints;

	UPROPERTY(EditDefaultsOnly)
		USphereComponent* ActivationSphere;
	UPROPERTY(EditDefaultsOnly)
		USkeletalMeshComponent* ObeliskMesh;

	UPROPERTY(BlueprintAssignable, Category = "Charge Widget")
		FChargeDelegate UpdateChargeWidget;

	UPROPERTY(BlueprintAssignable, Category = "Charge Widget")
		FActivationDelegate EnableCharge;

	UPROPERTY(BlueprintAssignable, Category = "Charge Widget")
		FActivationDelegate DisableCharge;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnComponentEndOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	void SpawnDecal();

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ADecalActor> ChargeZoneDecalClass;
	ADecalActor* ChargeZoneDecal;

	const float PlayerDetectionRadius = 750.0f;
	const float EnemyDetectionRadius = 2500.0f;

	const float TotalRequiredCharge = 10.0f;
	float ObeliskCharge = 0;

	//False if another Obelisk is already charging
	bool bCanBeginCharging = true;

	EActivationState ObeliskState = EActivationState::Inactive;
	AVoidSpokenGameModeBase* GameMode;

};
