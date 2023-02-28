// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "BaseItem.generated.h"


UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_None = 0    UMETA(DisplayName = "None"),	
	EIT_Health = 1  UMETA(DisplayName = "Health"),	
	EIT_Focus = 2   UMETA(DisplayName = "Focus"),	
	EIT_Stamina = 3 UMETA(DisplayName = "Stamina"),	
};

UCLASS()
class VOIDSPOKEN_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseItem();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item | Collision")
	class USphereComponent* Collision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Particles")
	class UNiagaraComponent* Particles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	EItemType Type = EItemType::EIT_None;

	UFUNCTION(BlueprintPure, BlueprintCallable)
	EItemType GetItemType() const { return Type; };
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
