// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPlatform.h"


// Sets default values
APlayerPlatform::APlayerPlatform()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerPlatform::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

// Called every frame
void APlayerPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}


//used with power up replacing tiles
void APlayerPlatform::ReEnableXTiles(int noToReEnable)
{
	int i = 0;
	while (i <= PlayerBoard.Num() && noToReEnable != 0)
	{
		if (PlayerBoard[i]->currentState == TileState::TILE_DESTROYED)
		{
			PlayerBoard[i]->ReEnableTile();
			--noToReEnable;
		}
		++i;
	}
}

void APlayerPlatform::ResetTileState()
{
	for (int i = 0; i < PlayerBoard.Num(); ++i)
	{
		if (PlayerBoard[i]->currentState == TileState::TILE_DESTROYED)
			PlayerBoard[i]->ReEnableTile();
	}
}
