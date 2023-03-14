// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DamageTypeStagger.h"
#include "SpikeProjectile.generated.h"

UCLASS()
class VOIDSPOKEN_API ASpikeProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpikeProjectile();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Projectile)
		UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = Projectile)
		USphereComponent* CollisionComponent;

	UFUNCTION(BlueprintImplementableEvent)
		void PlaySoundAtLocation(USoundCue* SoundToPlay, FVector SoundLocation, FRotator SoundRotation);

	UFUNCTION(BlueprintImplementableEvent)
		void DeactivateParticleSystem();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound Cues")
		USoundCue* ProjectileHittingCue;

	void SetDamage(float NewDamage, bool Collision = true, float LifeTime = 3);
	void SetSpeed(float InitialSpeed);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		void OnComponentBeginOverlap(class UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FTimerHandle DeactivationTimer;

	float Damage = 3;
	bool UseCollision = true;
};
