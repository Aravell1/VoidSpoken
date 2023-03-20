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
enum class EActivationState : uint8
{
	Inactive	UMETA(DisplayName = "Inactive"),
	Charging	UMETA(DisplayName = "Charging"),
	Activated	UMETA(DisplayName = "Activated")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FChargeDelegate);
UDELEGATE()
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FActivationDelegate, AObelisk*, Obelisk);

UCLASS()
class VOIDSPOKEN_API AObelisk : public APawn, public ITelekinesisInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AObelisk();

	UFUNCTION(BlueprintPure, Category = "Obelisk")
	FString GetObeliskName() const { return ObeliskName; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ObeliskName;

	UFUNCTION(BlueprintPure)
		EActivationState GetObeliskState() { return ObeliskState; }

	UFUNCTION(BlueprintCallable)
		void SetObeliskState(EActivationState NewState);

	UFUNCTION(BlueprintPure)
		float GetObeliskCharge() { return ObeliskCharge; }

	void BeginCharging();

	void SetCanBeginCharging(bool CanCharge) { bCanBeginCharging = CanCharge; }

	void Highlight_Implementation(bool bHighlight) override;

	UFUNCTION()
		void SpawnEnemyParticleEffect(ABaseEnemy* EnemyTrigger);

	UFUNCTION(BlueprintCallable)
		void AddCharge();

	UPROPERTY(EditAnywhere)
		TArray<AEnemyPortalSpawn*> NearbySpawnPoints;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		USphereComponent* ActivationSphere;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		USkeletalMeshComponent* ObeliskMesh;

	UPROPERTY(BlueprintAssignable, Category = "Charge Widget")
		FChargeDelegate UpdateChargeWidget;

	UPROPERTY(BlueprintAssignable, Category = "Obelisk State")
		FActivationDelegate EnableCharge;

	UPROPERTY(BlueprintAssignable, Category = "Obelisk State")
		FActivationDelegate DisableCharge;

	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<AActor> EnemyParticleType;

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

	const float EnemyDetectionRadius = 3500.0f;

	float ObeliskCharge = 0;

	//False if another Obelisk is already charging
	bool bCanBeginCharging = true;

	EActivationState ObeliskState = EActivationState::Inactive;
	AVoidSpokenGameModeBase* GameMode;

};
