// Fill out your copyright notice in the Description page of Project Settings.

#include "MainSourceGameModeBase.h"
#include "PlayerCharacterController.h"
#include "PlayerControllerMaster.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "PlayerControllerMaster.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Public/TimerManager.h"
#include "Camera/CameraActor.h"

#include "GameManager.h"
#include "Kismet/GameplayStatics.h"
#include "MainGameInstance.h"

AMainSourceGameModeBase::AMainSourceGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.
	bStartWithTickEnabled = true;

	/** defaults */
	playerIndex = 1;
	LivesPerTeam = 3;

	DebugMode = false;
	NumRounds = 1;
	DelayTimeS = 1.0f;
	CountdownTimeS = 3.0f;

	RoundTimeS = 93.f;

	currentRound = 1;
	
	TeamOneDrawPos.Add(FVector(0, 0, 0));
	TeamOneDrawPos.Add(FVector(100, 0, 0));
	TeamTwoDrawPos.Add(FVector(200, 0, 0));
	TeamTwoDrawPos.Add(FVector(300, 0, 0));
	DrawCamPos = FVector(1000, 1000, 1000);

	test = false;

	ScorePerTile = 10;

	static ConstructorHelpers::FObjectFinder<USoundCue>pThemeCueFinder(TEXT("SoundCue'/Game/Music/playtheme_Cue.playtheme_Cue'"));
	if (pThemeCueFinder.Succeeded())
	{
		pThemeCue = pThemeCueFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue>pTitleCueFinder(TEXT("SoundCue'/Game/Music/titlescreentheme_Cue.titlescreentheme_Cue'"));
	if (pTitleCueFinder.Succeeded())
	{
		pTitleCue = pTitleCueFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue>pCountdownCueFinder(TEXT("SoundCue'/Game/SFX/Countdown_Cue.Countdown_Cue'"));
	if (pCountdownCueFinder.Succeeded())
	{
		pCountdownCue = pCountdownCueFinder.Object;
	}

	WinStateEnum = EWinState::VE_Other;
	GameStateEnum = EGameState::VE_Debug;
}

void AMainSourceGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> p1;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerControllerMaster::StaticClass(), p1);

	
	APlayerControllerMaster* p = Cast<APlayerControllerMaster>(p1[0]);
	if (!DebugMode)
	{
		// Get the main game manager - probably easier way but i cant find it out
		TArray<AActor*> foundManagers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGameManager::StaticClass(), foundManagers);

		if (foundManagers.Num() != 0)
		{
			MainGameManager = Cast<AGameManager>(foundManagers[0]);

			if (MainGameManager)
			{
				UE_LOG(LogTemp, Warning, TEXT("Main Game Manager Located"));
			}
		}

		// Get all the components
		MainCamera = MainGameManager->MainCam;
		MainCamPos = MainCamera->GetActorLocation();

		KillTrigger = MainGameManager->KillZone;
		
	
		//StartGame();
	}
	else
	{
		GameStateEnum = EGameState::VE_Debug;
	}



	//UMainGameInstance* SGI = Cast<UMainGameInstance>(GetGameInstance());

	//UGameplayStatics::RemovePlayer(UGameplayStatics::GetPlayerController(GetWorld(), 1), true);

	//if (SGI)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("SINGLE GAME INSTANCE"));

	//	for (int i = 0; i < SGI->Team.Num(); i++)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Player ID: %d, Team ID: %d "), SGI->Team[i].playerID, SGI->Team[i].TeamID);

	//		// If this is the first player
	//		if (SGI->Team[i].playerID == 0)
	//		{
	//			if (SGI->Team[i].TeamID == 1)
	//			{
	//				TeamOne.TeamMembers.Add(p);
	//				p->pTeam = &TeamOne;
	//				p->ControllerIndex = 0;
	//				p->DisableInput(nullptr);
	//				p->SetPlayerColor(TeamOne.TeamColor);
	//				p->SetActorLocation(TeamOne.SpawnLocations[TeamOne.currentSpawnLocation]);
	//				TeamOne.currentSpawnLocation++;
	//			}
	//			else if (SGI->Team[i].TeamID == 2)
	//			{
	//				TeamTwo.TeamMembers.Add(p);
	//				p->pTeam = &TeamTwo;
	//				p->ControllerIndex = 0;
	//				p->DisableInput(nullptr);
	//				p->SetPlayerColor(TeamTwo.TeamColor);
	//				p->SetActorLocation(TeamTwo.SpawnLocations[TeamTwo.currentSpawnLocation]);
	//				TeamTwo.currentSpawnLocation++;
	//			}

	//			actors.Add(p);
	//		}
	//		else
	//		{
	//			if (SGI->Team[i].TeamID == 1)
	//			{
	//				SpawnPlayer(TeamOne, SGI->Team[i].playerID);
	//			}
	//			else if (SGI->Team[i].TeamID == 2)
	//			{
	//				SpawnPlayer(TeamTwo, SGI->Team[i].playerID);
	//			}
	//		}
	//	}
	//}
	
	if (!test)
	{
		TeamTwo.TeamMembers.Add(p);
		p->pTeam = &TeamTwo;
		p->ControllerIndex = 0;
		p->DisableInput(nullptr);
		p->SetPlayerColor(TeamTwo.TeamColor);
		p->pBillboardComponent->SetSprite(MainGameManager->PlayerIndicators[0]);

		ogSpawn.Add(p->GetActorLocation());
		actors.Add(p);

		SpawnPlayer(TeamOne, playerIndex);
		SpawnPlayer(TeamOne, playerIndex);
		SpawnPlayer(TeamTwo, playerIndex);
		test = true;
	}


	TArray<AActor*> tiles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATile::StaticClass(), tiles);
	for (AActor* tile : tiles)
	{
		ATile* t = Cast<ATile>(tile);
		if (t->TileTeamEnum == ETileTeam::VE_TileBlue)
			TeamOne.teamTiles.Add(t);
		if (t->TileTeamEnum == ETileTeam::VE_TileRed)
			TeamTwo.teamTiles.Add(t);
	}

	/*TeamOne.teamTiles = MainGameManager->BlueTeamTiles;
	TeamTwo.teamTiles = MainGameManager->RedTeamTiles;*/

	acTitle = UGameplayStatics::SpawnSound2D(this, pTitleCue, 1);

	/*if (p->pTeam == &TeamOne)
		TeamOne.TeamMembers[0]->SetActorLocation(TeamOne.SpawnLocations[0]);
	else
		TeamTwo.TeamMembers[0]->SetActorLocation(TeamTwo.SpawnLocations[0]);*/


}

void AMainSourceGameModeBase::StartGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Disabled Player"));

	GameStateEnum = EGameState::VE_Start;

	acTitle->SetActive(false);
	
	// Start the first timer
	//GetWorldTimerManager().SetTimer(GameTimingHandle, this, &AMainSourceGameModeBase::StartCountDown, DelayTimeS, false);

	// Disable all the players
	for (auto player : TeamOne.TeamMembers)
		player->DisablePlayer();

	for (auto player : TeamTwo.TeamMembers)
		player->DisablePlayer();
	RoundStart();

	UGameplayStatics::PlaySound2D(this, pCountdownCue, 1, 1, 0);

}

void AMainSourceGameModeBase::StartCountDown()
{
	UE_LOG(LogTemp, Warning, TEXT("Starting Countdown"));

	GameStateEnum = EGameState::VE_Countdown;

	//GetWorldTimerManager().SetTimer(GameTimingHandle, this, &AMainSourceGameModeBase::RoundStart, CountdownTimeS, false);

}

void AMainSourceGameModeBase::RoundStart()
{
	UE_LOG(LogTemp, Warning, TEXT("Starting Round"));

	GameStateEnum = EGameState::VE_InProgress;
	TArray<AActor*> terminals;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATerminal::StaticClass(), terminals);

	GetWorldTimerManager().SetTimer(GameTimingHandle, this, &AMainSourceGameModeBase::RoundEndTimer, RoundTimeS, false);

	// Disable all the players

	ATerminal* t;
	// Enable* all the players
	for (auto player : TeamOne.TeamMembers)
	{
		t = Cast<ATerminal>(terminals[1]);
		for (int i = 0; i < terminals.Num(); ++i)
		{
			t = Cast<ATerminal>(terminals[i]);
			if (t->teamID == 1)
			{
				player->pTerminal = t;
				UE_LOG(LogTemp, Warning, TEXT("Terminal Found"));
			}
		}
		player->EnablePlayer();
		player->EnableInput(nullptr);
	}
	
	for (auto player : TeamTwo.TeamMembers)
	{
		for (int i = 0; i < terminals.Num(); ++i)
		{
			t = Cast<ATerminal>(terminals[i]);
			if (t->teamID == 2)
			{
				player->pTerminal = t;
				UE_LOG(LogTemp, Warning, TEXT("Terminal Found"));
			}
		}
		player->EnablePlayer();
		player->EnableInput(nullptr);
	}

	UGameplayStatics::PlaySound2D(this, pThemeCue, 1, 1, 0);
}


// Called when the time has run out for the game
void AMainSourceGameModeBase::RoundEndTimer()
{
	// As the time limit has been reached, check the score

	TeamOne.Score = MainGameManager->GetTilesLost(1) * ScorePerTile;
	TeamTwo.Score = MainGameManager->GetTilesLost(2) * ScorePerTile;

	if (TeamOne.Score < TeamTwo.Score)
		WinStateEnum = EWinState::VE_TeamOneWin;
	else if (TeamTwo.Score < TeamOne.Score)
		WinStateEnum = EWinState::VE_TeamTwoWin;
	else
		WinStateEnum = EWinState::VE_Draw;

	RoundEnd();
}

void AMainSourceGameModeBase::RoundEnd()
{
	UE_LOG(LogTemp, Warning, TEXT("Ending Round"));

	GameStateEnum = EGameState::VE_RoundEnd;

	switch (WinStateEnum)
	{
	case EWinState::VE_TeamOneWin:
		TeamOne.TeamWins++;
		break;
	case EWinState::VE_TeamTwoWin:
		TeamTwo.TeamWins++;
		break;
	case EWinState::VE_Draw:
		TeamOne.TeamWins++;
		TeamTwo.TeamWins++;
		break;
	case EWinState::VE_Other:
		UE_LOG(LogTemp, Warning, TEXT("Why are we here? Just to suffer...You Should Never Get This Log Message!"));
		break;
	default:
		break;
	}

	// Check who won that round
	if (currentRound == NumRounds)
	{
		EndGame();
	}
	else
	{
		currentRound++;
		ResetMap();
		StartCountDown();
	}
}

void AMainSourceGameModeBase::EndGame()
{
	UE_LOG(LogTemp, Warning, TEXT("Ending Game"));

	GameStateEnum = EGameState::VE_GameEnd;

	// Disable all the players
	for (auto player : TeamOne.TeamMembers)
		player->EnablePlayer();

	for (auto player : TeamTwo.TeamMembers)
		player->EnablePlayer();

	// Now reset the map
	//ResetMap();

	// Check who won, hopefully team one - GO BLUE
	if (TeamOne.TeamWins == TeamTwo.TeamWins)
	{
		// Draw, do some draw thingy
		for (int i = 0; i < TeamOne.TeamMembers.Num(); i++)
			TeamOne.TeamMembers[i]->SetActorLocation(TeamOneDrawPos[i]);

		for (int i = 0; i < TeamTwo.TeamMembers.Num(); i++)
			TeamTwo.TeamMembers[i]->SetActorLocation(TeamTwoDrawPos[i]);

		MainCamera->SetActorLocation(DrawCamPos);
		MainCamera->SetActorRotation(FRotator(0, 0, -40));
		//UE_LOG(LogTemp, Warning, TEXT("Draw"));

	}
	else if (TeamOne.TeamWins > TeamTwo.TeamWins)
	{
		// Team one won
		for (int i = 0; i < TeamOne.TeamMembers.Num(); i++)
			TeamOne.TeamMembers[i]->SetActorLocation(TeamOne.TeamWinPos[i]);

		for (int i = 0; i < TeamTwo.TeamMembers.Num(); i++)
			TeamTwo.TeamMembers[i]->SetActorLocation(TeamOne.TeamLossPos[i]);

		MainCamera->SetActorLocation(TeamOneWinCamPos);
		MainCamera->SetActorRotation(FRotator(0, 0, -40));
		//UE_LOG(LogTemp, Warning, TEXT("Team One Wins"));
	}
	else
	{
		// Team two won
		for (int i = 0; i < TeamTwo.TeamMembers.Num(); i++)
			TeamTwo.TeamMembers[i]->SetActorLocation(TeamTwo.TeamWinPos[i]);

		for (int i = 0; i < TeamOne.TeamMembers.Num(); i++)
			TeamOne.TeamMembers[i]->SetActorLocation(TeamTwo.TeamLossPos[i]);

		MainCamera->SetActorLocation(TeamTwoWinCamPos);
		MainCamera->SetActorRotation(FRotator(0, 0, -40));
		//UE_LOG(LogTemp, Warning, TEXT("Team Two Wins"));
	}

	// Do some UI Stuff

	//ResetMap();

	FTimerHandle timer;
	FTimerDelegate del;
	del.BindLambda([&]
	{
		ResetMap();
		ExitGameFull();
		//UGameplayStatics::OpenLevel(GetWorld(), "MainMenu", false);
	});

	GetWorldTimerManager().SetTimer(timer, del, 5.f, false);

	//UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
}

void AMainSourceGameModeBase::HandleGameUpdate()
{
	bool TeamOneDead = false, TeamTwoDead = false;

	// If team one has no lifes left
	if (TeamOne.LivesLeft == 0)
	{
		int team_one_players_left_alive = 0;

		// And both players are no more :(
		for (int i = 0; i < TeamOne.TeamMembers.Num(); i++)
		{
			if (TeamOne.TeamMembers[i]->GetDead())
				team_one_players_left_alive++;
		}

		if (team_one_players_left_alive == TeamOne.TeamMembers.Num())
		{
			// Then this team is dead
			TeamOneDead = true;
		}
	}
	else if (TeamTwo.LivesLeft == 0) {

		int team_two_players_left_alive = 0;

		// And both players are no more :(
		for (int i = 0; i < TeamTwo.TeamMembers.Num(); i++)
		{
			if (TeamOne.TeamMembers[i]->GetDead())
				team_two_players_left_alive++;
		}

		if (team_two_players_left_alive == TeamTwo.TeamMembers.Num())
		{
			// Then this team is dead
			TeamTwoDead = true;
		}
	}

	if (TeamOneDead && !TeamTwoDead)
	{
		// Add a win onto team
		WinStateEnum = EWinState::VE_TeamTwoWin;
		UE_LOG(LogTemp, Warning, TEXT("Team Two Wins Round"));

		// Clear the timer
		GetWorldTimerManager().ClearTimer(GameTimingHandle);

		// End the round
		RoundEnd();
	}
	else if (!TeamOneDead && TeamTwoDead) {

		WinStateEnum = EWinState::VE_TeamOneWin;
		UE_LOG(LogTemp, Warning, TEXT("Team One Wins Round"));

		GetWorldTimerManager().ClearTimer(GameTimingHandle);
		RoundEnd();

	}
	else if (TeamOneDead && TeamTwoDead) {

		// If this happens then some shit has broken so class as a draw
		WinStateEnum = EWinState::VE_Draw;
		UE_LOG(LogTemp, Warning, TEXT("Draw Round (Broken though)"));

		GetWorldTimerManager().ClearTimer(GameTimingHandle);
		RoundEnd();

	}
	else
	{
		WinStateEnum = EWinState::VE_Other;
	}
}

void AMainSourceGameModeBase::HandleKillzoneCollision()
{
	for (auto t1 : TeamOne.TeamMembers)
		if (KillTrigger->IsOverlappingActor(t1) && !t1->GetDead())
			t1->Die();

	for (auto t2 : TeamTwo.TeamMembers)
		if (KillTrigger->IsOverlappingActor(t2) && !t2->GetDead())
			t2->Die();
}

void AMainSourceGameModeBase::ResetMap()
{
	UE_LOG(LogTemp, Warning, TEXT("Resetting Game"));
	for (int i = 0; i < 4; ++i)
	{
		UE_LOG(LogTemp, Warning, TEXT("POS: %s %d"), *ogSpawn[i].ToString())
		actors[i]->SetActorLocation(ogSpawn[i]);
	}
	UE_LOG(LogTemp, Warning, TEXT("cuckmas"));

	MainCamera->SetActorLocation(MainCamPos);
	MainGameManager->ResetTilePositions();
	TArray<AActor*> terms;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATerminal::StaticClass(), terms);

	for (AActor* a : terms)
	{
		ATerminal* t = Cast<ATerminal>(a);
		t->Unpossess();
	}


}

// TODO: Implement this to stop any possible bugs (will take some time)
void AMainSourceGameModeBase::HandleTileUpdate()
{
	
}

void AMainSourceGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GameStateEnum == EGameState::VE_InProgress)
	{
		HandleGameUpdate();
		HandleKillzoneCollision();
		HandleTileUpdate();
	}
}

// This is used when the player would like to play again
void AMainSourceGameModeBase::ResetAll()
{
	ResetMap();

	// Team One
	TeamOne.LivesLeft = 3;
	TeamOne.Score = 0.0f;
	TeamOne.TeamWins = 0;

	// Team Two
	TeamTwo.LivesLeft = 3;
	TeamTwo.Score = 0.0f;
	TeamTwo.TeamWins = 0;

	// Win State
	WinStateEnum = EWinState::VE_Other;

	currentRound = 1;

	// Restart the session
	StartGame();
}

void AMainSourceGameModeBase::SpawnPlayer(FTeam& TeamToJoin, int ControllerIndex)
{
	class UWorld* const world = GetWorld();
	FRotator spawnRotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters spawnParams;
	spawnParams.Owner = this;
	spawnParams.Instigator = AActor::GetInstigator();//Instigator;
	//UE_LOG(LogTemp, Warning, TEXT("Player %d"), TeamToJoin.currentSpawnLocation);
	if (world)
	{
		// Create a new player controller and assign to the chosen controller
		class APlayerController* NewPlayerController = Cast<APlayerController>(UGameplayStatics::CreatePlayer(world, ControllerIndex));
	
		if (NewPlayerController)
		{
			class APlayerControllerMaster* CreatedPlayer = Cast<APlayerControllerMaster>(NewPlayerController->GetCharacter());
			if (CreatedPlayer)
			{
				CreatedPlayer->Destroy();
				NewPlayerController->UnPossess();
				FVector spawnPos = TeamToJoin.SpawnLocations[TeamToJoin.currentSpawnLocation];
				UE_LOG(LogTemp, Warning, TEXT("POS: %s %d"), *spawnPos.ToString(), TeamToJoin.currentSpawnLocation);
				class APlayerControllerMaster* NewPlayer = world->SpawnActor<APlayerControllerMaster>(DefaultPawnClass, spawnPos, spawnRotation, spawnParams);
				if (NewPlayer)
				{
					UE_LOG(LogTemp, Warning, TEXT("Spawning Player"));
					NewPlayerController->Possess(NewPlayer);
					TeamToJoin.TeamMembers.Add(NewPlayer);
					NewPlayer->pTeam = &TeamToJoin;
					NewPlayer->SetPlayerColor(TeamToJoin.TeamColor);
					NewPlayer->pBillboardComponent->SetSprite(MainGameManager->PlayerIndicators[ControllerIndex]);
					TeamToJoin.currentSpawnLocation++;
					NewPlayerController->DisableInput(nullptr);
					NewPlayer->ControllerIndex = playerIndex;
					ogSpawn.Add(spawnPos);
					actors.Add(NewPlayer);
				}
				
			}
		}
	}
	playerIndex++;
}