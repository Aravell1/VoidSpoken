// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Math/UnrealMathUtility.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GhoulAIController.generated.h"

class UBehaviorTreeComponent;

/**
 * 
 */
UCLASS()
class VOIDSPOKEN_API AGhoulAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AGhoulAIController();
	virtual void SeePlayer(APawn* Player);


protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
		TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
		TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
		TObjectPtr<UBlackboardComponent> BlackboardComponent;
};
