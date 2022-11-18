// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Math/UnrealMathUtility.h"
#include "GatekeeperAIController.generated.h"

class UBehaviorTreeComponent;

/**
 * 
 */


UCLASS()
class VOIDSPOKEN_API AGatekeeperAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AGatekeeperAIController();
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
