// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TriggerVolume.h"
#include "Camera/CameraActor.h"
#include "GameFramework/Actor.h"
#include "Tile.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

#include "GameManager.generated.h"

UCLASS()
class MAINSOURCE_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** The trigger volume used for checking whether character position is out of bounds */
	UPROPERTY(EditAnywhere)
		ATriggerVolume* KillZone;

	/** The Camera Poisition for Team 2*/
	UPROPERTY(EditAnywhere)
		ACameraActor* MainCam;

	/** The Camera Poisition for Team 2*/
	UPROPERTY(EditAnywhere)
		ACameraActor* GameOverCam;

	/** Tiles relating to the red team */
	UPROPERTY(EditAnywhere)
	TArray<ATile*> RedTeamTiles;

	/** Tiles relating to the blue team */
	UPROPERTY(EditAnywhere)
	TArray<ATile*> BlueTeamTiles;

	/** UnAssigned Tiles */
	UPROPERTY(EditAnywhere)
	TArray<ATile*> UATeamTiles;

	UPROPERTY(EditAnywhere)
		AActor* TeamOneCannon;

	UPROPERTY(EditAnywhere)
		AActor* TeamOneTerminal;

	UPROPERTY(EditAnywhere)
		AActor* TeamTwoCannon;

	UPROPERTY(EditAnywhere)
		AActor* TeamTwoTerminal;

	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic* DynamicMaterialInst;

	UPROPERTY(EditAnywhere)
	TArray<UTexture2D*> PlayerIndicators;

public:

	void ResetTilePositions();
	void SetActorColor(AActor* actorToChange, FColor newColor);
	int GetTilesLost(int TeamID);

private:

	TArray<FVector> TilePositions;


};
