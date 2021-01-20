// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacterController.h"
#include "ConstructorHelpers.h"
#include "Components/InputComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Public/TimerManager.h"
#include "Bullet.h"

constexpr float kMaxSpeed = 50.0f;

APlayerCharacterController::APlayerCharacterController()
{
	// Set this pawn to call Tick() every frame.
	PrimaryActorTick.bCanEverTick = true;

	// By Default we get some extra bindings (e.g. jump bound to up).
	bAddDefaultMovementBindings = false;

	// Set uo the root 
	pMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComponent"));
	pMeshComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshFinder(TEXT("StaticMesh'/Game/Assets/Models/character_export.character_export'"));
	if (pMeshComponent && meshFinder.Succeeded())
	{
		pMeshComponent->SetStaticMesh(meshFinder.Object);
		pMeshComponent->SetRelativeLocation(FVector(0, 0, 0));
	}


	// Create a SpringArmComponent
	pSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	pSpringArm->SetupAttachment(RootComponent);
	pSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 100.0f), FRotator(-60.0f, -90.0f, 0.f));
	pSpringArm->TargetArmLength = 2000.f;
	pSpringArm->bEnableCameraLag = true;
	pSpringArm->bInheritYaw = false;
	pSpringArm->CameraLagSpeed = 3.0f;

	// Create a CameraComponent
	pCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	pCamera->SetupAttachment(pSpringArm, USpringArmComponent::SocketName);

	// Set up the test variable
	RightStickXInput = 0;
	RightStickYInput = 0;

	// Set up player dash variables
	bCanDash = true;
	DashDelay = 2.0f;
	DashPower = 5000.0f;

	// Cannon Stuff
	build = false;
}

void APlayerCharacterController::SetPos(FVector newPos)
{
	RootComponent->SetWorldLocation(newPos);
}

void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();
}

float mult = 1.0f;

void APlayerCharacterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Cannon Build Power
	if (build)
	{
		power += .5f * mult;

		if (power > 100.0f)
			mult = -1.f;
		
		if (power < 0.f)
			mult = 1.f;
	}

	HandleRotation();
}

void APlayerCharacterController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind the player actions
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerCharacterController::OnFire);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayerCharacterController::Dash);

	// Bind the player Axis
	PlayerInputComponent->BindAxis("XAxisL", this, &APlayerCharacterController::MoveXAxisL);
	PlayerInputComponent->BindAxis("YAxisL", this, &APlayerCharacterController::MoveYAxisL);
	PlayerInputComponent->BindAxis("XAxisR", this, &APlayerCharacterController::MoveXAxisR);
	PlayerInputComponent->BindAxis("YAxisR", this, &APlayerCharacterController::MoveYAxisR);

	// Cannon Controls
	PlayerInputComponent->BindAction("CannonMoveRight", IE_Pressed, this, &APlayerCharacterController::MoveCannonRight);
	PlayerInputComponent->BindAction("CannonMoveLeft", IE_Pressed, this, &APlayerCharacterController::MoveCannonLeft);
	PlayerInputComponent->BindAction("CannonMoveRight", IE_Released, this, &APlayerCharacterController::StopCannon);
	PlayerInputComponent->BindAction("CannonMoveLeft", IE_Released, this, &APlayerCharacterController::StopCannon);
	PlayerInputComponent->BindAction("CannonFire", IE_Pressed, this, &APlayerCharacterController::BuildCannonPower);
	PlayerInputComponent->BindAction("CannonFire", IE_Released, this, &APlayerCharacterController::ShootCannon);
}

void APlayerCharacterController::HandleRotation()
{
	FVector tempVelocity = FVector(RightStickXInput, RightStickYInput, 0);
	FRotator tempRotator = tempVelocity.Rotation();

	// NOTE: This is a temp fix
	if (RightStickXInput > 0 || RightStickYInput > 0)
	{
		SetActorRotation(tempRotator);
		PlayerLastRotation = tempRotator;

	}
	else
	{
		SetActorRotation(PlayerLastRotation);
	}

}

void APlayerCharacterController::OnFire()
{
	// Spawn a Bullet actor in front of the ship.
	FVector Location(GetActorLocation() + GetActorForwardVector() * 200);
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfo;
	ABullet* pBullet = GetWorld()->SpawnActor<ABullet>(Location, Rotation, SpawnInfo);

	// Setup the Bullet's velocity.
	pBullet->SetVelocity(GetActorForwardVector() * 20);
}

void APlayerCharacterController::Dash()
{
	// Check whether the player is able to dash
	if (bCanDash)
	{
		bCanDash = false;

		GetWorldTimerManager().SetTimer(DashTimerHandle, this, &APlayerCharacterController::ResetDash, DashDelay, false);

		const FVector ForwardDirection = this->GetActorRotation().Vector();

		this->GetMovementComponent()->Velocity += ForwardDirection * DashPower;
	}
}


float APlayerCharacterController::TakeDamage(float _damage, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	health -= _damage;

	if (health <= 0)
	{
		Die();
		return 0.0f;
	}
	return 0.0f;
}

void APlayerCharacterController::Die()
{
	// What to do when the player dies

}

void APlayerCharacterController::MoveXAxisL(float value)
{
	GetMovementComponent()->AddInputVector(FVector(kMaxSpeed, 0.f, 0.f) * value);
}

void APlayerCharacterController::MoveYAxisL(float value)
{
	GetMovementComponent()->AddInputVector(FVector(0.f, -kMaxSpeed, 0.f) * value);
}

void APlayerCharacterController::MoveXAxisR(float value)
{
	RightStickXInput = value;
}

void APlayerCharacterController::MoveYAxisR(float value)
{
	RightStickYInput = value;
}

//Cannon Stuff
void APlayerCharacterController::MoveCannonLeft()
{
	pCannon->RotateLeft();
}
void APlayerCharacterController::MoveCannonRight()
{
	pCannon->RotateRight();
}
void APlayerCharacterController::StopCannon()
{
	pCannon->StopRotating();
}
void APlayerCharacterController::ShootCannon()
{
	build = false;
	pCannon->FireAction(power);
}
void APlayerCharacterController::BuildCannonPower()
{
	power = 0.0f;
	build = true;
}

// Some Helpers
void APlayerCharacterController::ResetDash() { bCanDash = true; }
float APlayerCharacterController::GetHealth() { return health; }
void APlayerCharacterController::SetHealth(float _newHealth) { health = _newHealth; }


