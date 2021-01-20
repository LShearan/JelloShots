// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TeamSelectionCharacter.h"
#include "Engine/Texture2D.h"
#include "TeamSelectionManager.h"
#include "TeamSelectionGMB.generated.h"

/**
*
*/

USTRUCT()
struct FTeamSelect
{
	GENERATED_USTRUCT_BODY()

		/** This Team Member */
		ATeamSelectionCharacter* TSPlayer;

	/** The Textur It Uses */
	UTexture2D* PlayerIndicatorTex;

	/** The ID of the player indicator */
	int TexID;

	/** Team ID */
	int TeamID;

	/** Ready */
	bool Ready;

	/** defaults */
	FTeamSelect()
	{
		TexID = 0;
		TeamID = 0;
		Ready = false;
	};
};

UCLASS()
class MAINSOURCE_API ATeamSelectionGMB : public AGameModeBase
{
	GENERATED_BODY()

public:

	ATeamSelectionGMB();

protected:

	virtual void BeginPlay() override;

public:

	virtual void Tick(float DeltaTime) override;

private:

	/** The Manager */
	ATeamSelectionManager * TSManager;
	//UMainGameInstance* GInstance;

	TArray<ATeamSelectionCharacter*> PlayersTS;
	TArray<FTeamSelect> Players;

	TArray<FTeamSelect> TeamOnePlayers;
	TArray<FTeamSelect> TeamTwoPlayers;

	/** Some Debug Stuff */
	FTimerHandle ReportTimerHandle;
	float reportTime = 10.0f;
	bool CanReport = true;
	void ResetReport() { CanReport = true; }
	void DebugReport();
	void SetUpPlayers(FVector SpawnPos, int ControllerIndex);
};
