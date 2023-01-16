// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "JsonLibrary.h"
#include "StatsMasterClass.h"
#include "PlayerCharacter.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Actor.h"
#include "CheckPoints.generated.h"

UCLASS()
class VOIDSPOKEN_API ACheckPoints : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckPoints();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoints")
	class USphereComponent* CheckpointMeshCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoints")
	class UStaticMeshComponent* CheckpointMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoints")
	class UJsonLibrary* JsonLibrary;

	float MeshRadius;

	UFUNCTION()
	void OnPlayerCollide(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reference")
	AActor* ReferenceActor;

private:

	UFUNCTION()
	void OnSpawnEnemy();
};
