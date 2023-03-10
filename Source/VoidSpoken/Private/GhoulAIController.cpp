// Fill out your copyright notice in the Description page of Project Settings.


#include "GhoulAIController.h"

AGhoulAIController::AGhoulAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree Component"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard Component"));
}

void AGhoulAIController::SeePlayer(APawn* Player)
{
	SetFocus(Player);
}

void AGhoulAIController::FindLocationWithLOS(UEnvQuery* LOSQuery)
{
	UE_LOG(LogTemp, Warning, TEXT("Query Called"));

	FEnvQueryRequest LineOfSightQueryRequest = FEnvQueryRequest(LOSQuery, GetPawn());
	LineOfSightQueryRequest.Execute(EEnvQueryRunMode::SingleResult, this, &AGhoulAIController::MoveToQueryResult);
}

void AGhoulAIController::MoveToQueryResult(TSharedPtr<FEnvQueryResult> result)
{
	if (result->IsSuccessful()) {
		MoveToLocation(result->GetItemAsLocation(0));
	}
}

void AGhoulAIController::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(BehaviorTree.Get()))
	{
		RunBehaviorTree(BehaviorTree.Get());
		BehaviorTreeComponent->StartTree(*BehaviorTree.Get());
	}

}

void AGhoulAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsValid(Blackboard.Get()) && IsValid(BehaviorTree.Get()))
	{
		Blackboard->InitializeBlackboard(*BehaviorTree.Get()->BlackboardAsset.Get());
	}
}