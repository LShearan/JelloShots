// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainGameInstance.generated.h"

/**
 * 
 */

USTRUCT()
struct FPlayerTeam
{
	GENERATED_USTRUCT_BODY()

public:

	/** Which player */
	int playerID;

	/** Which Team */
	int TeamID;

	/** defaults */
	FPlayerTeam()
	{
		playerID = 0;
		TeamID = 0;
	};
};

UCLASS()
class MAINSOURCE_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	
	TArray<FPlayerTeam> Team;

};
