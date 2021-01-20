// Fill out your copyright notice in the Description page of Project Settings.

#include "Cannon.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"

// Sets default values
ACannon::ACannon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComponent"));
	pMeshComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>meshFinder(TEXT("StaticMesh'/Game/Assets/Models/Cannon_Cannon_Top.Cannon_Cannon_Top'"));

	if (pMeshComponent && meshFinder.Succeeded())
	{
		pMeshComponent->SetStaticMesh(meshFinder.Object);
		pMeshComponent->SetRelativeLocation(FVector(0, 0, 0));
		pMeshComponent->SetRelativeRotation(GetActorRotation());
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> pCannonFireCueFinder(TEXT("SoundCue'/Game/SFX/Cannon_Fire_Cue.Cannon_Fire_Cue'"));
	if (pCannonFireCueFinder.Succeeded())
	{
		pCannonFireCue = pCannonFireCueFinder.Object;
	}

	pBillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
	pBillboardComponent->SetupAttachment(pMeshComponent);

	YawSpeed = 0.0f;
	fireRefreshTimer = 2.0f;
	maxPower = 420.f;
	minPower = 240.f;
	bCanFire = true;
}

// Called when the game starts or when spawned
void ACannon::BeginPlay()
{
	Super::BeginPlay();
	
	MaxYaw = GetActorRotation().Yaw + 60.0f;
	MinYaw = GetActorRotation().Yaw - 60.0f;
}

// Called every frame
void ACannon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float OldYaw = GetActorRotation().Yaw;
	const float MinDeltaYaw = MinYaw - OldYaw;
	const float MaxDeltaYaw = MaxYaw - OldYaw;

	FRotator DeltaRotation(0, 0, 0);
	DeltaRotation.Yaw = FMath::ClampAngle(YawSpeed*DeltaTime, MinDeltaYaw, MaxDeltaYaw);

	AddActorWorldRotation(DeltaRotation);
}


void ACannon::RotateRight()
{
	SetYawSpeed(-30.0f);
}

void ACannon::RotateLeft()
{
	SetYawSpeed(30.0f);
}

void ACannon::StopRotating()
{
	SetYawSpeed(0.0f);
}

void ACannon::FireAction(float power)
{

	if (bCanFire)
	{
		const float Power = power;
		if (power > maxPower)
			power = maxPower;
		else if (power < minPower)
			power = minPower;

		testPower = power;

		UGameplayStatics::PlaySoundAtLocation(this, pCannonFireCue, GetActorLocation(), 1, 1);

		FVector Location = pBillboardComponent->GetComponentTransform().GetLocation();;
		FRotator Rotation = GetActorRotation();
		FActorSpawnParameters SpawnInfo;
		AProjectile* pProjectile = GetWorld()->SpawnActor<AProjectile>(Location, Rotation, SpawnInfo);

		pProjectile->SetVelocity(GetActorForwardVector() * Power);
		pProjectile->MyComp->AddImpulse(GetActorForwardVector());
		bCanFire = false;

		GetWorldTimerManager().SetTimer(FireRateHandle, this, &ACannon::ResetCanFire, fireRefreshTimer, false);

	}
}

void ACannon::ResetCanFire()
{
	bCanFire = true;
}