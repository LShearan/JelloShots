// Fill out your copyright notice in the Description page of Project Settings.

#include "TeamSelectionCharacter.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "MainGameInstance.h"
#include "TeamSelectionGMB.h"

// Sets default values
ATeamSelectionCharacter::ATeamSelectionCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pBillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
	pBillboardComponent->SetupAttachment(GetMesh());

	PlayerReady = false;
	PlayerVisible = false;
	PlayerML = true;
	PlayerMR = true;
	InputCanMove = true;
	InputMoveTimer = 0.5;
	TexIndicatorID = 0;
	CanStart = false;
	id = 0;

	PlayerTeamState = EPlayerState::VE_DEFAULTTEAM;
}

// Called when the game starts or when spawned
void ATeamSelectionCharacter::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Player Joined %d"), GetController());

	StartingPos = GetActorLocation();

	/** For testing */
	SetColor(FColor::Purple);

	DefaultColor = GetColor();

	// Set the mesh be invisible
	SetVisibility(false);

	TArray<AActor*> foundManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATeamSelectionManager::StaticClass(), foundManagers);

	if (foundManagers.Num() != 0)
	{
		MainGameManager2 = Cast<ATeamSelectionManager>(foundManagers[0]);

		if (MainGameManager2)
		{
			if (MainGameManager2->PlayerIndicators.Num() != 0)
			{
				TexIndicatorID = FMath::RandRange(0, MainGameManager2->PlayerIndicators.Num() - 1);
				this->pBillboardComponent->SetSprite(MainGameManager2->PlayerIndicators[TexIndicatorID]);
			}
		}

	}
}

// Called every frame
void ATeamSelectionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ATeamSelectionCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// The action of pressing x / enter
	PlayerInputComponent->BindAction("TeamSelectionJoin", IE_Pressed, this, &ATeamSelectionCharacter::XAction);
	PlayerInputComponent->BindAction("TeamSelectionLeave", IE_Pressed, this, &ATeamSelectionCharacter::BAction);

	PlayerInputComponent->BindAction("DPAD_Left", IE_Pressed, this, &ATeamSelectionCharacter::MoveRight);
	PlayerInputComponent->BindAction("DPAD_Right", IE_Pressed, this, &ATeamSelectionCharacter::MoveLeft);
	PlayerInputComponent->BindAxis("XAxisL", this, &ATeamSelectionCharacter::MoveAxis);
}

void ATeamSelectionCharacter::MoveAxis(float value)
{
	if (InputCanMove)
	{
		if (value > 0.5)
		{
			UE_LOG(LogTemp, Warning, TEXT("Input Big"));
			MoveLeft();
			InputCanMove = false;
			GetWorldTimerManager().SetTimer(InputHandle, this, &ATeamSelectionCharacter::ResetInputCanMove, InputMoveTimer, false);

		}
		else if (value < -0.5)
		{
			UE_LOG(LogTemp, Warning, TEXT("Input Smol"));
			MoveRight();
			InputCanMove = false;
			GetWorldTimerManager().SetTimer(InputHandle, this, &ATeamSelectionCharacter::ResetInputCanMove, InputMoveTimer, false);
		}
	}
}

void ATeamSelectionCharacter::MoveLeft()
{
	FVector PlayerCurrentLocation = this->GetActorLocation();

	switch (PlayerTeamState)
	{
	case EPlayerState::VE_REDTEAM:
		break;
	case EPlayerState::VE_DEFAULTTEAM:
		if (PlayerVisible)
		{
			this->SetActorLocation(FVector(PlayerCurrentLocation.X + MovementValue, PlayerCurrentLocation.Y, PlayerCurrentLocation.Z));
			PlayerTeamState = EPlayerState::VE_REDTEAM;
			TeamID = 2;
			SetColor(FColor::Red);
		}
		break;
	case EPlayerState::VE_BLUETEAM:
		if (PlayerVisible)
		{
			this->SetActorLocation(FVector(PlayerCurrentLocation.X + MovementValue, PlayerCurrentLocation.Y, PlayerCurrentLocation.Z));
			PlayerTeamState = EPlayerState::VE_DEFAULTTEAM;
			TeamID = 0;
			SetColor(DefaultColor);
		}
		break;
	default:
		break;
	}
}

void ATeamSelectionCharacter::MoveRight()
{
	FVector PlayerCurrentLocation = this->GetActorLocation();

	switch (PlayerTeamState)
	{
	case EPlayerState::VE_REDTEAM:
		if (PlayerVisible)
		{
			this->SetActorLocation(FVector(PlayerCurrentLocation.X - MovementValue, PlayerCurrentLocation.Y, PlayerCurrentLocation.Z));
			PlayerTeamState = EPlayerState::VE_DEFAULTTEAM;
			TeamID = 0;
			SetColor(DefaultColor);
		}
		break;
	case EPlayerState::VE_DEFAULTTEAM:
		if (PlayerVisible)
		{
			this->SetActorLocation(FVector(PlayerCurrentLocation.X - MovementValue, PlayerCurrentLocation.Y, PlayerCurrentLocation.Z));
			PlayerTeamState = EPlayerState::VE_BLUETEAM;
			TeamID = 1;
			SetColor(FColor::Blue);
		}
		break;
	case EPlayerState::VE_BLUETEAM:
		break;
	default:
		break;
	}
}

void ATeamSelectionCharacter::XAction()
{
	if (!PlayerVisible)
	{
		SetVisibility(true);
		PlayerVisible = true;
		PlayerReady = true;
		SetActorLocation(StartingPos);
		PlayerTeamState = EPlayerState::VE_DEFAULTTEAM;
		return;
	}
	else if (PlayerVisible && PlayerReady && CanStart)
	{
		StartGame();
	}
}

void ATeamSelectionCharacter::BAction()
{
	if (!PlayerVisible && !PlayerReady)
		return;
	else if (PlayerVisible)
	{
		SetVisibility(false);
		PlayerVisible = false;
		PlayerReady = false;
		SetActorLocation(StartingPos);
		PlayerTeamState = EPlayerState::VE_DEFAULTTEAM;
		return;
	}
}

void ATeamSelectionCharacter::SetVisibility(bool visible)
{
	GetMesh()->SetVisibility(visible);
	pBillboardComponent->SetVisibility(visible);
}

void ATeamSelectionCharacter::SetReady(bool ready) {
	PlayerReady = ready;
}

void ATeamSelectionCharacter::SetColor(FColor newColor)
{
	UMaterialInterface * Material = this->GetMesh()->GetMaterial(0);
	UMaterialInstanceDynamic* matInstance = GetMesh()->CreateDynamicMaterialInstance(0, Material);

	if (matInstance != nullptr)
		matInstance->SetVectorParameterValue("Subsurface_Colour", newColor);
}

FColor ATeamSelectionCharacter::GetColor()
{
	/** Still need to figure this out */
	FColor ColorToReturn = FColor::Purple;
	return ColorToReturn;
}

int ATeamSelectionCharacter::GetTexIndicatorID()
{
	return TexIndicatorID;
}

void ATeamSelectionCharacter::StartGame()
{
	UMainGameInstance* SGI = Cast<UMainGameInstance>(GetGameInstance());

	if (SGI)
	{
		/** Get all the characters */
		TArray<AActor*> FoundCharacters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATeamSelectionCharacter::StaticClass(), FoundCharacters);

		if (FoundCharacters.Num() != 0)
		{
			for (int i = 0; i < FoundCharacters.Num(); i++)
			{
				ATeamSelectionCharacter* tempCharacter = Cast<ATeamSelectionCharacter>(FoundCharacters[i]);
				FPlayerTeam tempChar;

				tempChar.playerID = tempCharacter->id;
				tempChar.TeamID = tempCharacter->TeamID;
				SGI->Team.Add(tempChar);
			}
		}
	}

	LoadLevelBP();
}