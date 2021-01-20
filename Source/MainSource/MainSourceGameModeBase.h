// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include"Runtime/Engine/Classes/Sound/SoundCue.h"
#include "GameFramework/GameModeBase.h"
#include "Components/AudioComponent.h"
#include "Materials/Material.h"
#include "Engine/TriggerVolume.h"
#include "Tile.h"
#include "MainSourceGameModeBase.generated.h"

/**
 *
 */

class APlayerControllerMaster;
class ACameraActor;
class AGameManager;

UENUM(BlueprintType)
enum class EGameState : uint8
{
	VE_Debug		UMETA(DisplayName = "Debug"),
	VE_Start		UMETA(DisplayName = "Start"),
	VE_Countdown	UMETA(DisplayName = "Countdown"),
	VE_InProgress	UMETA(DipslayName = "InProgress"),
	VE_RoundEnd		UMETA(DisplayName = "RoundEnd"),
	VE_GameEnd		UMETA(DisplayName = "GameEnd")
};

UENUM(BlueprintType)
enum class EWinState : uint8
{
	VE_TeamOneWin	UMETA(DisplayName = "Team One Win"),
	VE_TeamTwoWin	UMETA(DisplayName = "Team One Win"),
	VE_Draw			UMETA(DisplayName = "Draw"),
	VE_Other		UMETA(DisplayName = "Other")
};

USTRUCT()
struct FTeam
{
	GENERATED_USTRUCT_BODY()

	/** The name of the team - for the UI */
	UPROPERTY(EditDefaultsOnly, Category = "Team Config")
	FString TeamName;

	/** The color of the team - for the material */
	UPROPERTY(EditDefaultsOnly, Category = "Team Config")
	FColor TeamColor;

	/** The array of spawn locations */
	UPROPERTY(EditDefaultsOnly, Category = "Team Config")
	TArray<FVector> SpawnLocations;

	/** The remaining lives of the team */
	UPROPERTY(VisibleAnywhere, Category = "Team Config")
	int LivesLeft;

	/** The score lives of the team */
	UPROPERTY(VisibleAnywhere, Category = "Team Config")
	float Score;

	/** The array of players on the team */
	UPROPERTY(VisibleAnywhere, Category = "Team Config")
	TArray<APlayerControllerMaster*> TeamMembers;

	/** The amount of wins */
	UPROPERTY(VisibleAnywhere, Category = "Team Config")
	int TeamWins;

	/** The end game win positions */
	UPROPERTY(EditDefaultsOnly, Category = "Team Config")
	TArray<FVector> TeamWinPos;

	/** The end game loss positions */
	UPROPERTY(EditDefaultsOnly, Category = "Team Config")
	TArray<FVector> TeamLossPos;

	

	int currentSpawnLocation;

	TArray<ATile*> teamTiles;
	
	/** defaults */
	FTeam()
	{
		TeamName = "DefaultTeam";
		TeamColor = FColor(255, 0, 0, 255);
		SpawnLocations.Add(FVector(0, 0, 0));
		LivesLeft = 3;
		Score = 0.0f;
		TeamWinPos.Add(FVector(4960.0, 20870.0, 18618.0));
		TeamWinPos.Add(FVector(-3350.0, 22730.0, 18618.0));
		TeamLossPos.Add(FVector(500.0, 22360.0, 18618.0));
		TeamLossPos.Add(FVector(2290.0, 24190.0, 18618.0));
		currentSpawnLocation = 0;
	}
};

UCLASS()
class MAINSOURCE_API AMainSourceGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMainSourceGameModeBase();

	class USoundCue* pThemeCue;
	class USoundCue* pTitleCue;
	class USoundCue* pCountdownCue;

	UAudioComponent* acTitle;
	//UAudioComponent* acTheme;
protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team Config")
	APlayerControllerMaster* CharacterToSpawn;

	/** The amount of lives each team starts with */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team Config")
		int LivesPerTeam;

	UPROPERTY(EditDefaultsOnly, Category = "Team Config")
		FTeam TeamOne;

	UPROPERTY(EditDefaultsOnly, Category = "Team Config")
		FTeam TeamTwo;

	UPROPERTY(EditAnywhere, Category = "Gameplay")
	ATriggerVolume* KillTrigger;

protected:

	/** Enabling this will stop all timings taking place */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing")
	bool DebugMode;

	/** How many rounds should be played? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing")
	int NumRounds;

	/** The amount of time before the game countdown starts */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing")
	float DelayTimeS;

	/** The amount of time in the countdown */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing")
	float CountdownTimeS;

	/** How long is each round? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timing")
	float RoundTimeS;

	/** This will keep track of all the timings needed for the gameplay */
	FTimerHandle GameTimingHandle;

protected:

	// Avengers End Game Spoilers Ahead (Not Really, Just End Game Stuff)

	UPROPERTY(EditDefaultsOnly, Category = "End Game")
	float ScorePerTile;

	/** The positions for a draw, Team 1*/
	UPROPERTY(EditDefaultsOnly, Category = "End Game")
	TArray<FVector> TeamOneDrawPos;

	/** The positions for a draw, Team 2*/
	UPROPERTY(EditDefaultsOnly, Category = "End Game")
	TArray<FVector> TeamTwoDrawPos;

	/** The Camera Poisition for Team 1*/
	UPROPERTY(EditDefaultsOnly, Category = "End Game")
	FVector TeamOneWinCamPos;

	/** The Camera Poisition for Team 2*/
	UPROPERTY(EditDefaultsOnly, Category = "End Game")
	FVector TeamTwoWinCamPos;

	/** The Camera Poisition for Team 2*/
	UPROPERTY(EditDefaultsOnly, Category = "End Game")
	FVector DrawCamPos;

	/** The Main Camera */
	UPROPERTY(VisibleAnywhere, Category = "End Game")
	ACameraActor* MainCamera;
	
	/** The Game Manager */
	UPROPERTY(VisibleAnywhere, Category = "End Game")
	AGameManager* MainGameManager;

	/** The Default Camera Pos */
	FVector MainCamPos;

protected:

	/** Enums that keep track of the game state and win state */
	UPROPERTY(EditDefaultsOnly, Category = "Game Mode")
	EGameState GameStateEnum;

	UPROPERTY(VisibleAnywhere, Category = "Game Mode")
	EWinState WinStateEnum;

public:

	virtual void Tick(float DeltaTime) override;

	void SpawnPlayer(FTeam& TeamToJoin, int ControllerIndex);

	UFUNCTION(BlueprintCallable)
	void StartGame();

	UFUNCTION(BlueprintCallable)
	float getRoundTime() {
		return RoundTimeS;
	}

	UFUNCTION(BlueprintImplementableEvent)
		void ExitGameFull();

	UFUNCTION(BlueprintCallable)
		FString getResult() {
		FString res;
		res = "U BUMBDER";
		switch (WinStateEnum)
		{
		case EWinState::VE_TeamOneWin:
			res = "TEAM BLUE";
			break;

		case EWinState::VE_TeamTwoWin:
			res = "TEAM RED";
			break;

		case EWinState::VE_Draw:
			res = "NO";
			break;
		}
		return res;
	}

	UFUNCTION(BlueprintCallable)
		bool gameOver()
	{
		if (GameStateEnum == EGameState::VE_GameEnd)
			return true;
		return false;
	}

	void StartCountDown();
	void RoundStart();
	void RoundEnd();
	void RoundEndTimer();
	void ResetMap();
	void ResetAll();
	void EndGame();

	void HandleGameUpdate();
	void HandleKillzoneCollision();
	void HandleTileUpdate();

private:

	/** The current index of the last next player*/
	int playerIndex;
	int currentRound;

	TArray<FVector> ogSpawn;
	TArray<APlayerControllerMaster*> actors;
	bool test;
};