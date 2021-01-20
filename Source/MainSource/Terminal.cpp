// Fill out your copyright notice in the Description page of Project Settings.

#include "Terminal.h"
#include "Engine/StaticMesh.h"

#include "Runtime/Engine/Public/TimerManager.h"

#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#include "Particles/ParticleSystemComponent.h"

#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"

#include "PlayerControllerMaster.h"

// Sets default values
ATerminal::ATerminal()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	// Set up Static mesh Component with Assest
	pMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComponent"));
	pMeshComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh>meshFinder(TEXT("StaticMesh'/Game/Assets/Models/TerminalModel.TerminalModel'"));

	if (pMeshComponent && meshFinder.Succeeded())
	{
		pMeshComponent->SetStaticMesh(meshFinder.Object);
		pMeshComponent->SetRelativeLocation(FVector(0,0,0));
		pMeshComponent->SetRelativeRotation(FRotator(0,0,0));
	}

	pMeshComponent->SetNotifyRigidBodyCollision(true);

	// Find and set the three shield particle systems Green,Red and Yellow to be used for Sheild Particle
	static ConstructorHelpers::FObjectFinder<UParticleSystem> gFinder(TEXT("ParticleSystem'/Game/Shield/PS_GreenShield.PS_GreenShield'"));
	if (gFinder.Succeeded())
	{
		pGreenParticle = gFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> yFinder = ConstructorHelpers::FObjectFinder<UParticleSystem>(TEXT("ParticleSystem'/Game/Shield/PS_YellowShield.PS_YellowShield'"));

	if (yFinder.Succeeded())
	{
		pYellowParticle = yFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> rFinder = ConstructorHelpers::FObjectFinder<UParticleSystem>(TEXT("ParticleSystem'/Game/Shield/PS_RedShield.PS_RedShield'"));
	if(rFinder.Succeeded())
	{
		pRedParticle = rFinder.Object;
	}


	pShieldParticle = pGreenParticle;


	// Intialise Varibles
	power = 195.f;
	bActive = false;
	bCanShoot = true;
	collided = false;
	build = false;
	mult = 1.0f;
	lives = 3;
	minPower = 240.f;
	maxPower = 420.f;
}

// Called when the game starts or when spawned
void ATerminal::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATerminal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Build Cannon Power
		if (build)
		{
			power += 1.f * mult;

			if (power > maxPower)
			{
				power = maxPower;
				mult = -1.0f;
			}

			if (power < minPower)
			{
				power = minPower;
				mult = 1.0f;
			}
		}
}

void ATerminal::Hit()
{
	--lives;

	if (lives < 0)
	{
		lives = 3;
		Unpossess();
	}

	switch (lives)
	{
	case 1:
		pShieldParticle = pRedParticle;
		break;
	case 2:
		pShieldParticle = pYellowParticle;
		break;
	case 3:
		pShieldParticle = pGreenParticle;
		break;
	default:
		break;
	}
}

void ATerminal::RotateLeft()
{
	pCannon->RotateLeft();
}
void ATerminal::RotateRight()
{
	pCannon->RotateRight();
}
void ATerminal::StopRotating()
{
	pCannon->StopRotating();
}

void ATerminal::ShootCannon()
{
	if (bCanShoot)
	{
		build = false;
		bCanShoot = false;
		pCannon->FireAction(power);
		power = minPower;

		GetWorldTimerManager().SetTimer(tTimeBetweenShots, this, &ATerminal::ResetFire, 3.f, false);
	}
}

void ATerminal::BuildPower()
{
	if(bCanShoot)
		build = true;
}

void ATerminal::Unpossess()
{
	if (pPawn)
	{

		APlayerControllerMaster* player = Cast<APlayerControllerMaster>(pPawn);
		if (player)
		{
			player->bUsing = false;
			player->bCanMove = true;
		}

		APlayerController* controller = UGameplayStatics::GetPlayerController(this, playerId);
		controller->bAutoManageActiveCameraTarget = false;
		controller->Possess(pPawn);
		pPawn = nullptr;
		bActive = false;
		power = minPower;
		bCanShoot = true;
		collided = false;
		build = false;
		mult = 1.0f;
		lives = 3;

	}

}

// Called to bind functionality to input
void ATerminal::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAction("CannonMoveRight", IE_Pressed, this, &ATerminal::RotateRight);
	PlayerInputComponent->BindAction("CannonMoveLeft", IE_Pressed, this, &ATerminal::RotateLeft);
	PlayerInputComponent->BindAction("CannonMoveRight", IE_Released, this, &ATerminal::StopRotating);
	PlayerInputComponent->BindAction("CannonMoveLeft", IE_Released, this, &ATerminal::StopRotating);
	PlayerInputComponent->BindAction("CannonFire", IE_Pressed, this, &ATerminal::BuildPower);
	PlayerInputComponent->BindAction("CannonFire", IE_Released, this, &ATerminal::ShootCannon);
	PlayerInputComponent->BindAction("Possess", IE_Pressed, this, &ATerminal::Unpossess);
}

void ATerminal::ResetFire()
{
	bCanShoot = true;
}

void ATerminal::SpawnEmitter()
{
	// Spawn a particle system
	FVector Location(GetActorLocation());
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfo;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), pShieldParticle, Location, Rotation, true);

}