// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerControllerMaster.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Classes/Engine/LevelScriptActor.h"
#include "Runtime/Engine/Classes/Engine/TargetPoint.h"
#include "Runtime/Engine/Public/EngineUtils.h"
#include "Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "Tile.h"

// Sets default values
APlayerControllerMaster::APlayerControllerMaster(const FObjectInitializer &ObjectInitializer)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set Character Movement Component Variables
	pCharacterMovement = GetCharacterMovement();
	pCharacterMovement->JumpZVelocity = 600.f;
	pCharacterMovement->GravityScale = 1.f;
	pCharacterMovement->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	pCharacterMovement->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate

	pBillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
	pBillboardComponent->SetupAttachment(GetMesh());

	/** defaults */
	bIsDead = false;
	bCanDisable = false;
	bCanRespawn = false;
	bCanMove = true;
	bRolling = false;
	bUsing = false;
	bHit = false;
	m_respawnCountdown = 3.0f;
	bCanDash = true;
	DashDelay = 3.0f;
	MovementDelay = 1.f;
	DashPower = 50000.0f;
	RightStickXInput = 0;
	RightStickYInput = 0;
	m_playerMaxHealth = 100;
	m_playerHealth = m_playerMaxHealth; 
	m_respawnCountdown = 3.0f;
	m_playerDeathHeight = -1000;
	ControllerIndex = 0;
	MaxSpeed = 500.0f;
}

// Called when the game starts or when spawned
void APlayerControllerMaster::BeginPlay()
{
	Super::BeginPlay();
	
	FVector Location(0.0f, 0.0f, 0.0f);
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfo;
	pGun = GetWorld()->SpawnActor<AGun>(Location, Rotation, SpawnInfo);
	
	pGun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName(TEXT("Ws")));
}

// Called every frame
void APlayerControllerMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleMovement(DeltaTime);
	//HandleRotation();
	CheckKillHeight();
}

// Called to bind functionality to input
void APlayerControllerMaster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind the player actions
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &APlayerControllerMaster::OnFire);
	PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &APlayerControllerMaster::Dash);
	PlayerInputComponent->BindAction("Possess", IE_Pressed, this, &APlayerControllerMaster::PossessCannon);

	// Bind the player Axis
	PlayerInputComponent->BindAxis("XAxisL", this, &APlayerControllerMaster::MoveXAxisL);
	PlayerInputComponent->BindAxis("YAxisL", this, &APlayerControllerMaster::MoveYAxisL);
	PlayerInputComponent->BindAxis("XAxisR", this, &APlayerControllerMaster::MoveXAxisR);
	PlayerInputComponent->BindAxis("YAxisR", this, &APlayerControllerMaster::MoveYAxisR);

	// Set up the debug button 
	PlayerInputComponent->BindAction("DebugButton", IE_Pressed, this, &APlayerControllerMaster::Die);
}

// Use this to test various aspects
void APlayerControllerMaster::Debugging()
{
	DamagePlayer(25.0f);
}

void APlayerControllerMaster::HandleMovement(float DeltaTime)
{
	if (!bIsDead)
	{
		if (bCanMove)
		{
			// Find movement direction
			const float ForwardValue = GetInputAxisValue("YAxisL");
			const float RightValue = GetInputAxisValue("XAxisL");

			// Clamp max size so that (X=1, Y=1) doesn't cause faster movement in diagonal directions
			const FVector TempDirection = FVector(ForwardValue, RightValue, 0.f).GetClampedToMaxSize(1.0f);
			const FVector MoveDirection = UKismetMathLibrary::RotateAngleAxis(TempDirection, -40.f, FVector(0.f, 0.f, 1.f));

			// Calculate  movement
			const FVector Movement = MoveDirection * 5000.f * DeltaTime;

			if (Movement.SizeSquared() > 0.f)
			{
				const FRotator NewRotation = Movement.Rotation();
				PlayerLastRotation = NewRotation;

				GetMovementComponent()->AddInputVector(Movement);
				SetActorRotation(NewRotation);


			}
			else
			{
				SetActorRotation(PlayerLastRotation);
			}
		}
		else
		{
			SetActorRotation(PlayerLastRotation);
		}
	}
}

void APlayerControllerMaster::HandleRotation()
{
	if (!bIsDead && bCanMove)
	{
		const float FireForwardValue = -RightStickYInput;
		const float FireRightValue = RightStickXInput;

		const FVector TempDirection = FVector(FireForwardValue, FireRightValue, 0.f);
		const FVector RotationDirection = UKismetMathLibrary::RotateAngleAxis(TempDirection, -40.f, FVector(0.f, 0.f, 1.f));

		SetActorRotation(RotationDirection.Rotation());
	}
}

void APlayerControllerMaster::Dash()
{
	if (bCanDash)
	{
		bCanDash = false;
		bCanMove = false;
		bRolling = true;

		GetWorldTimerManager().SetTimer(DashTimerHandle, this, &APlayerControllerMaster::ResetDash, DashDelay, false);

		GetWorldTimerManager().SetTimer(MovementTimer, this, &APlayerControllerMaster::CommitDash, MovementDelay, false);

	}
}

void APlayerControllerMaster::CommitDash()
{
	const FVector ForwardDirection = GetActorForwardVector().GetSafeNormal();

	this->GetMovementComponent()->Velocity += ForwardDirection * DashPower;

	bRolling = false;
	bCanMove = true;
}

void APlayerControllerMaster::EndDash()
{
	bRolling = false;
	bCanMove = true;
}
void APlayerControllerMaster::OnFire()
{
	pGun->Fire();
}

void APlayerControllerMaster::DisablePlayer()
{
	if (GetMesh())
	{
		this->GetMesh()->SetActive(false);
		bCanDash = false;
	}
}

void APlayerControllerMaster::EnablePlayer()
{
	if (GetMesh())
	{
		this->SetActorEnableCollision(true);
		this->GetMesh()->SetActive(true);
		bCanDash = true;
		bCanMove = true;
	}
}

void APlayerControllerMaster::DamagePlayer(float _damage)
{
	m_playerHealth -= _damage;

	UE_LOG(LogTemp, Warning, TEXT("Player takes %f damage and has %f health"), _damage, m_playerHealth);

	if (m_playerHealth <= 0)
	{
		m_playerHealth = 0;
		Die();
	}
}

void APlayerControllerMaster::GiveHealth(float _healingToGive)
{
	m_playerHealth += _healingToGive;

	UE_LOG(LogTemp, Warning, TEXT("Player gains %f health and has %f health"), _healingToGive, m_playerHealth);

	if (m_playerHealth >= m_playerMaxHealth)
		m_playerHealth = m_playerMaxHealth;
}

void APlayerControllerMaster::Die()
{
	// Disable the player
	DisablePlayer();
	this->SetActorEnableCollision(true);
	bIsDead = true;

	GetWorldTimerManager().SetTimer(RespawnTimer, this, &APlayerControllerMaster::PlayerRespawn, m_respawnCountdown, false);

	UE_LOG(LogTemp, Warning, TEXT("Playe Dead"));
}

void APlayerControllerMaster::PlayerRespawn()
{
	if (GetMesh())
	{
		EnablePlayer();
		bIsDead = false;
		
		int randomSpawn;
		FVector spawnLocation;

		TArray<ATile*> OpenTiles;

		for (auto tile : pTeam->teamTiles)
		{
			if (tile->currentState == TileState::TILE_ACTIVE && !tile->isBaseTile)
				OpenTiles.Add(tile);
		}

		randomSpawn = FMath::RandRange(0, OpenTiles.Num() - 1);
		spawnLocation = OpenTiles[randomSpawn]->GetActorLocation();
		this->SetActorLocation(FVector(spawnLocation.X,spawnLocation.Y, spawnLocation.Z + 300));
	}
}

void APlayerControllerMaster::CheckKillHeight()
{
	/*if (this->GetActorLocation().Z < m_playerDeathHeight && !bIsDead)
		Die();*/
}

void APlayerControllerMaster::MoveXAxisL(float value)
{
	if (!bIsDead && bCanMove)
	{
		const FVector tempDirection(0.f, MaxSpeed, 0.f);
		const FVector MoveDirection = UKismetMathLibrary::RotateAngleAxis(tempDirection, -40.f, FVector(0.f, 0.f, 1.f));

		//GetMovementComponent()->AddInputVector(MoveDirection * value);
	}
}

void APlayerControllerMaster::MoveYAxisL(float value)
{
	if (!bIsDead && bCanMove)
	{
		const FVector tempDirection(MaxSpeed, 0.f, 0.f);
		const FVector MoveDirection = UKismetMathLibrary::RotateAngleAxis(tempDirection, -40.f, FVector(0.f, 0.f, 1.f));

		//GetMovementComponent()->AddInputVector(MoveDirection * value);
	}
}

void APlayerControllerMaster::MoveXAxisR(float value)
{
	if (!bIsDead && bCanMove)
		RightStickXInput = value;
}

void APlayerControllerMaster::MoveYAxisR(float value)
{
	if (!bIsDead && bCanMove)
		RightStickYInput = value;

}

void APlayerControllerMaster::ResetDash() { bCanDash = true; }

void APlayerControllerMaster::PossessCannon()
{
	if (bAtStation)
	{
		APlayerController* controller = UGameplayStatics::GetPlayerController(this, ControllerIndex);
		if (pTerminal)
		{
			bUsing = true;
			bCanMove = false;
			APlayerController* controller = UGameplayStatics::GetPlayerController(this, ControllerIndex);

			SetActorLocation(pTerminal->GetActorLocation() + FVector(360.f, 920.f, 1808.0f));

			controller->bAutoManageActiveCameraTarget = false;
			pPlayer = this;
			controller->UnPossess();
			pTerminal->pPawn = pPlayer;
			controller->Possess(pTerminal);

			pTerminal->playerId = ControllerIndex;
			pTerminal->bActive = true;

		}
	}
}

// Returns whether the player is dead or not
bool APlayerControllerMaster::GetDead() { return bIsDead; }

// I know that is spelt colour wrong, but who cares, you're wrong for thinking that
void APlayerControllerMaster::SetPlayerColor(FColor newColor)
{
	// Avatar the last air bender is NOT ANIME
	UMaterialInterface * Material = this->GetMesh()->GetMaterial(0);
	UMaterialInstanceDynamic* matInstance = GetMesh()->CreateDynamicMaterialInstance(0, Material);

	if (matInstance != nullptr)
		matInstance->SetVectorParameterValue("Subsurface_Colour", newColor);

}