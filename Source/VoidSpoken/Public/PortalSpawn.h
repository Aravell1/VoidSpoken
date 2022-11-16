// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEnemy.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PortalSpawn.generated.h"

UCLASS()
class VOIDSPOKEN_API APortalSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalSpawn();
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
		TSubclassOf<ABaseEnemy> EnemyClass = ABaseEnemy::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Any)
		TArray<TSubclassOf<ABaseEnemy>> EnemyArray = {};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SpawnEnemy();

	int EnemyCount = 0;

};
