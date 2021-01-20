// Fill out your copyright notice in the Description page of Project Settings.

#include "TeamSelectionGMB.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

ATeamSelectionGMB::ATeamSelectionGMB()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATeamSelectionGMB::BeginPlay()
{
	Super::BeginPlay();

	/** Get the Manager */
	TArray<AActor*> foundManagers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATeamSelectionManager::StaticClass(), foundManagers);

	if (foundManagers.Num() != 0)
	{
		TSManager = Cast<ATeamSelectionManager>(foundManagers[0]);

		if (TSManager)
		{
			if (TSManager->PlayerIndicators.Num() != 0)
			{

			}
		}

	}

	int index = 1;
	SetUpPlayers(FVector(-22000, -3000, 18940), index++);
	SetUpPlayers(FVector(-22000, -6000, 18940), index++);
	SetUpPlayers(FVector(-22000, -9000, 18940), index++);

	/** Get all the characters */
	TArray<AActor*> FoundCharacters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATeamSelectionCharacter::StaticClass(), FoundCharacters);

	if (FoundCharacters.Num() != 0)
	{
		for (int i = 0; i < FoundCharacters.Num(); i++)
		{
			FTeamSelect tempChar;
			ATeamSelectionCharacter* tempCharacter = Cast<ATeamSelectionCharacter>(FoundCharacters[i]);

			tempChar.TSPlayer = tempCharacter;
			tempChar.PlayerIndicatorTex = TSManager->PlayerIndicators[tempCharacter->GetTexIndicatorID()];
			tempChar.TeamID = 0;
			tempChar.TexID = tempCharacter->GetTexIndicatorID();
			tempCharacter->pBillboardComponent->Sprite = TSManager->PlayerIndicators[tempCharacter->id];

			Players.Add(tempChar);

			UE_LOG(LogTemp, Warning, TEXT("Player: %s Added, Team ID: %d, Texture ID: %d"), *tempChar.TSPlayer->GetName(), tempChar.TeamID, tempChar.TexID);
		}
	}
}

void ATeamSelectionGMB::SetUpPlayers(FVector SpawnPos, int ControllerIndex)
{
	class UWorld* const world = GetWorld();
	FRotator spawnRotation(0.0f, 90.0f, 00.0f);
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	spawnParams.Instigator = AActor::GetInstigator();// Instigator;
	//UE_LOG(LogTemp, Warning, TEXT("Player %d"), TeamToJoin.currentSpawnLocation);
	if (world)
	{
		// Create a new player controller and assign to the chosen controller
		class APlayerController* NewPlayerController = Cast<APlayerController>(UGameplayStatics::CreatePlayer(world, ControllerIndex));

		if (NewPlayerController)
		{
			class ATeamSelectionCharacter* CreatedPlayer = Cast<ATeamSelectionCharacter>(NewPlayerController->GetCharacter());
			if (CreatedPlayer)
			{
				CreatedPlayer->Destroy();
				NewPlayerController->UnPossess();
				FVector spawnPos = SpawnPos;

				class ATeamSelectionCharacter* NewPlayer = world->SpawnActor<ATeamSelectionCharacter>(DefaultPawnClass, spawnPos, spawnRotation, spawnParams);

				if (NewPlayer)
				{
					NewPlayerController->Possess(NewPlayer);
					NewPlayer->id = ControllerIndex;
				}

			}
		}
	}
}

void ATeamSelectionGMB::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	int Team1 = 0;
	int Team2 = 0;
	bool CanReady = false;
	FString ReadyStr = "";

	for (auto pl : Players)
	{
		pl.TeamID = pl.TSPlayer->GetTeamID();

		if (pl.TeamID == 1)
			Team1 += 1;
		else if (pl.TeamID == 2)
			Team2 += 1;
	}

	if (Team1 != 0 && Team2 != 0)
	{
		CanReady = true;
		ReadyStr = "Yes";
	}
	else
	{
		CanReady = false;
		ReadyStr = "No";
	}

	for (auto pl : Players)
		pl.TSPlayer->SetStart(CanReady);

	//UE_LOG(LogTemp, Warning, TEXT("T1 %d T2 %d Can Ready: %s"), Team1, Team2, *ReadyStr);

	// Debugging
	if (CanReport)
		DebugReport();
}


void ATeamSelectionGMB::DebugReport()
{
	// Report the status of the player
	for (auto Player : Players)
	{
		Player.TexID = Player.TSPlayer->GetTexIndicatorID();
		Player.Ready = Player.TSPlayer->GetReady();
		Player.TeamID = Player.TSPlayer->GetTeamID();

		int TReady = 0;

		if (Player.Ready)
			TReady = 1;
		else
			TReady = 0;

		UE_LOG(LogTemp, Warning, TEXT("Player: %s Added, Team ID: %d, Ready: %d Texture ID: %d"), *Player.TSPlayer->GetName(), Player.TeamID, TReady, Player.TexID);
	}

	CanReport = false;

	// Report again later
	GetWorldTimerManager().SetTimer(ReportTimerHandle, this, &ATeamSelectionGMB::ResetReport, reportTime, false);
}