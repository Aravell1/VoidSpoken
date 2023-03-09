// Fill out your copyright notice in the Description page of Project Settings.


#include "TelekineticProp.h"

// Sets default values
ATelekineticProp::ATelekineticProp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	#pragma region Setup Static Mesh Component
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = StaticMesh;

	StaticMesh->SetSimulatePhysics(false);
	StaticMesh->SetNotifyRigidBodyCollision(true);
	StaticMesh->SetCollisionProfileName("Telekinesis");
	StaticMesh->CustomDepthStencilValue = 1;

	StaticMesh->SetAngularDamping(2.0f);
	#pragma endregion

	static ConstructorHelpers::FObjectFinder<UCurveFloat>C_LiftCurve(TEXT("/Game/Blueprints/Player/Telekinesis/Zoom.Zoom"));
	if (C_LiftCurve.Succeeded())
		LiftCurve = C_LiftCurve.Object;
}

void ATelekineticProp::PostInitializeComponents() {
	Super::PostInitializeComponents();

	StaticMesh->OnComponentHit.AddDynamic(this, &ATelekineticProp::OnComponentHit);
}

// Called when the game starts or when spawned
void ATelekineticProp::BeginPlay()
{
	Super::BeginPlay();

	#pragma region Setup Timeline
	FOnTimelineFloat LiftProgressUpdate;
	LiftProgressUpdate.BindUFunction(this, FName("LiftUpdate"));

	FOnTimelineEvent LiftFinishedEvent;
	LiftFinishedEvent.BindUFunction(this, FName("LiftFinished"));

	LiftTimeline.AddInterpFloat(LiftCurve, LiftProgressUpdate);
	LiftTimeline.SetTimelineFinishedFunc(LiftFinishedEvent);
	#pragma endregion
}

// Called every frame
void ATelekineticProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	LiftTimeline.TickTimeline(DeltaTime);
	if (State == ETelekinesisState::ETS_Pulled)	ReachCharacter();
}

#pragma region Highlights
void ATelekineticProp::Highlight_Implementation(bool bHighlight) {
	StaticMesh->SetRenderCustomDepth(bHighlight);
}
#pragma endregion

void ATelekineticProp::Pull_Implementation(ACharacter* Character) {
	Execute_Highlight(this, false);
	PlayerCharacter = Character;
	StaticMesh->SetSimulatePhysics(true);
	LiftOff();
}

void ATelekineticProp::Push_Implementation(FVector Destination, float Force) {
	PushTarget = Destination;
	State = ETelekinesisState::ETS_Pushed;
	StopLift();

	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	Player->SetTelekineticAttackState(ETelekinesisAttackState::ETA_None);

	//const FVector Impulse = UKismetMathLibrary::Multiply_VectorFloat(UKismetMathLibrary::GetDirectionUnitVector(GetActorLocation(), PushTarget), UKismetMathLibrary::MapRangeClamped(StaticMesh->GetMass(), 50.0f, 700.0f, 10.0f, 5.0f) * Force);
	const FVector Impulse = UKismetMathLibrary::Subtract_VectorVector(Destination, GetActorLocation()).GetSafeNormal() * UKismetMathLibrary::MapRangeClamped(StaticMesh->GetMass(), 50.0f, 700.0f, 10.0f, 5.0f) * Force;
	
	if (bEnableGravityDelay && !GetWorldTimerManager().IsTimerActive(EnableGravityDelay))
		GetWorldTimerManager().SetTimer(EnableGravityDelay, this, &ATelekineticProp::SetGravity, 0.3f, false);
	else StaticMesh->SetEnableGravity(true);
	
	StaticMesh->AddImpulse(Impulse, NAME_None, true);
	
	StaticMesh->SetLinearDamping(0.1f);
	StaticMesh->SetAngularDamping(0.0f);
}

void ATelekineticProp::Drop_Implementation() {
	PushTarget = FVector::ZeroVector;
	State = ETelekinesisState::ETS_Default;
	StopLift();

	StaticMesh->SetCollisionProfileName("Telekinesis");
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	Player->SetTelekineticAttackState(ETelekinesisAttackState::ETA_None);

	StaticMesh->SetEnableGravity(true);
	StaticMesh->SetLinearDamping(0.1f);
	StaticMesh->SetAngularDamping(0.0f);
}

#pragma region Visual Touches

void ATelekineticProp::LiftUpdate(const float Alpha) {
	const FVector NewLocation = FMath::Lerp(LiftStart, LiftEnd, Alpha);
	SetActorLocation(NewLocation);
}

void ATelekineticProp::LiftFinished() {
	StaticMesh->SetCollisionProfileName("Telekinesis");
	StaticMesh->AddAngularImpulseInDegrees(UKismetMathLibrary::Multiply_VectorFloat(PlayerCharacter->GetActorUpVector(), 800.0f), NAME_None, true);
	StaticMesh->SetEnableGravity(false);
	StaticMesh->SetLinearDamping(20.0f);
	StaticMesh->SetAngularDamping(2.0f);
	State = ETelekinesisState::ETS_Pulled;

}

void ATelekineticProp::ChaoticJitter() {
	JitterCounter++;
	if (JitterCounter >= 60) {
		JitterCounter = 0;
		StaticMesh->AddImpulse(UKismetMathLibrary::Multiply_VectorFloat(UKismetMathLibrary::RandomUnitVector(), 200.0f), NAME_None, true);
	}
}

void ATelekineticProp::ReachCharacter() {
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	const FVector Impulse = UKismetMathLibrary::Multiply_VectorFloat(UKismetMathLibrary::ClampVectorSize(UKismetMathLibrary::Subtract_VectorVector(Player->TelekinesisSource->GetComponentLocation(), GetActorLocation()), 0, 1000.0f), UKismetMathLibrary::MapRangeClamped(StaticMesh->GetMass(), 50.0f, 700.0f, 5.0f, 1.0f));
	StaticMesh->AddImpulse(Impulse, NAME_None, true);

	if (UKismetMathLibrary::Vector_Distance(GetActorLocation(), Player->TelekinesisSource->GetComponentLocation()) <= 50.0f)
		Player->SetTelekineticAttackState(ETelekinesisAttackState::ETA_Hold);

	ChaoticJitter();
}

void ATelekineticProp::LiftOff() {
	Highlight_Implementation(false);
	StaticMesh->SetCollisionProfileName("NoCollision");
	LiftStart = GetActorLocation();
	LiftEnd = GetActorLocation() + FVector(0, 0, 100);
	LiftTimeline.PlayFromStart();
}

void ATelekineticProp::StopLift() {
	LiftTimeline.Stop();
}

#pragma endregion

#pragma region Collision Events

void ATelekineticProp::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) {
	if (State == ETelekinesisState::ETS_Pushed) {
		StaticMesh->SetEnableGravity(true);
		if (State == ETelekinesisState::ETS_Pulled)
			Drop_Implementation();
		
		StaticMesh->SetAllPhysicsLinearVelocity(FVector());
		StaticMesh->AddImpulse(UKismetMathLibrary::Multiply_VectorFloat(NormalImpulse, 0.2f));
		State = ETelekinesisState::ETS_Default;
		
		UGameplayStatics::ApplyDamage(OtherActor, PropDamage, NULL, PlayerCharacter, NULL);
		if (!GetWorldTimerManager().IsTimerActive(SimulatePhysicsDelay))
			GetWorldTimerManager().SetTimer(SimulatePhysicsDelay, this, &ATelekineticProp::ToggleSimulatePhysics, 2.0f, true);
	}
}

#pragma endregion


