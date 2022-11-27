// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "StatsMasterClass.h"
#include "BaseWeapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseEntity.generated.h"


UCLASS()
class VOIDSPOKEN_API ABaseEntity : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABaseEntity();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Stats")
		class UStatsMasterClass* Stats;

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		virtual void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
