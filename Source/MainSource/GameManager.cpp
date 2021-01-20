// Fill out your copyright notice in the Description page of Project Settings.

#include "GameManager.h"
#include "Engine/Texture2D.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AGameManager::AGameManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGameManager::BeginPlay()
{
	Super::BeginPlay();
	
	// Get all the tiles
	TArray<AActor*> foundTiles;
	ATile* TempTile;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATile::StaticClass(), foundTiles);

	if (foundTiles.Num() != 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Tiles"));

		for (int i = 0; i < foundTiles.Num(); i++)
		{

			// Go through the tiles and assign them to the correct list
			TempTile = Cast<ATile>(foundTiles[i]);

			if (TempTile->TileTeamEnum == ETileTeam::VE_TileBlue)
				BlueTeamTiles.Add(TempTile);
			else if (TempTile->TileTeamEnum == ETileTeam::VE_TileRed)
				RedTeamTiles.Add(TempTile);
			else
				UATeamTiles.Add(TempTile);

			//UE_LOG(LogTemp, Warning, TEXT("Found Tile: %s"), *foundTiles[i]->GetName());
		}
			
	}
}

// Called every frame
void AGameManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGameManager::ResetTilePositions()
{
	// Reset the position of all the tiles in the world
	for (int i = 0; i < BlueTeamTiles.Num(); i++)
		BlueTeamTiles[i]->ReEnableTile();

	for (int i = 0; i < RedTeamTiles.Num(); i++)
		RedTeamTiles[i]->ReEnableTile();

	for (int i = 0; i < UATeamTiles.Num(); i++)
		UATeamTiles[i]->ReEnableTile();
}

int AGameManager::GetTilesLost(int teamID)
{
	int tilesLost = 0;

	switch (teamID)
	{
	case 1:
		for (auto tile : BlueTeamTiles)
		{
			if (tile->currentState == TileState::TILE_DESTROYED)
				tilesLost++;
		}
		break;

	case 2:
		for (auto tile : RedTeamTiles)
		{
			if (tile->currentState == TileState::TILE_DESTROYED)
				tilesLost++;
		}
		break;

	default:
		tilesLost = 0;
		break;
	};

	return tilesLost;
}

void AGameManager::SetActorColor(AActor* act, FColor nCol)
{

	//TArray<UStaticMeshComponent*> Components;
	//act->GetComponents<UStaticMeshComponent>(Components);
	//UStaticMeshComponent* StaticMeshComponent = Components[0];
	//UStaticMesh* StaticMesh = StaticMeshComponent->GetStaticMesh();

	//UMaterialInterface * Material1 = StaticMesh->GetMaterial(0);

	//DynamicMaterialInst = UMaterialInstanceDynamic::Create(Material1, StaticMesh);
	//DynamicMaterialInst->SetVectorParameterValue("Subsurface_Colour", nCol);

	//StaticMeshComponent->SetMaterial(0, DynamicMaterialInst);

	//UE_LOG(LogTemp, Warning, TEXT("This is a test on cannon %s"));

	//UMaterialInstanceDynamic* matInstance = StaticMesh->CreateDynamicMaterialInstance(0, Material);

	//// Avatar the last air bender is NOT ANIME
	//UMaterialInterface * Material = this->GetMesh()->GetMaterial(0);
	//UMaterialInstanceDynamic* matInstance = GetMesh()->CreateDynamicMaterialInstance(0, Material);

	//if (matInstance != nullptr)
	//	matInstance->SetVectorParameterValue("Subsurface_Colour", newColor);
}
