// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConstructorHelpers.h"
#include "Tile.generated.h"

UENUM()
enum class TileState : uint8
{
	TILE_ACTIVE UMETA(DisplayName="Active"),
	TILE_DESTROYED UMETA(DisplayName= "Destroyed")
};

UENUM(BlueprintType)
enum class ETileTeam : uint8
{
	VE_TileRed UMETA(DisplayName = "Red"),
	VE_TileBlue UMETA(DisplayName = "Blue"),
	VE_TileUA UMETA(DisplayName = "UnAssigned")
};

UCLASS()
class MAINSOURCE_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

	/** What team does this tile correspond to */
	UPROPERTY(EditAnywhere)
	ETileTeam TileTeamEnum;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ptr_TileMesh;
	
	UPROPERTY(EditAnywhere)
		int TileHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TileState currentState;

	UFUNCTION(BlueprintCallable)
	void HasBeenHit();
	void ReEnableTile();
	
	UPROPERTY(EditAnywhere)
	bool isBaseTile;

	float timer;
};
