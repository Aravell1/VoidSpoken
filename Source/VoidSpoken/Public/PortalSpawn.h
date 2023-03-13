// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseEnemy.h"
#include "Ghoul.h"
#include "Kismet/KismetMathLibrary.h"
#include "../VoidSpokenGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "CombatDirector.h"
#include "PortalSpawn.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackCommand);

UCLASS()
class VOIDSPOKEN_API APortalSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalSpawn();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
		void DisableEnemyHealthBar(ABaseEnemy* SpawnedEnemy);

	UPROPERTY(EditAnywhere)
		TSubclassOf<ABaseEnemy> EnemyClass = ABaseEnemy::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<TSubclassOf<ABaseEnemy>> EnemyArray = {};

	UPROPERTY(BlueprintAssignable)
		FAttackCommand OrderAttackBP;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void SpawnEnemy();

	ACombatDirector* CombatDirector;

};
