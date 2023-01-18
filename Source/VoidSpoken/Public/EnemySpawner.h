// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "PlayerCharacter.h"
#include "Engine/TargetPoint.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class VOIDSPOKEN_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemySpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Spawner")
	class UBoxComponent* SpawnArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Spawner")
	TArray<AActor*> SpawnPointReference;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Spawner")
	TSubclassOf<AActor> SpawnEnemy;

	UFUNCTION(BlueprintCallable, Category = "MyCategory")
	FVector GetActorLocations(TArray<AActor*> Actors);

private:

	UFUNCTION()
	void OnStartEnemySpawn(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

};
