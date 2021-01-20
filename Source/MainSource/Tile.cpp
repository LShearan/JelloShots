// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"


// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ptr_TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	ptr_TileMesh->SetupAttachment(RootComponent);
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshFinder(TEXT("StaticMesh'/Game/Assets/Map_and_Props/MEsh/tesselhex.tesselhex'"));
	if (ptr_TileMesh && meshFinder.Succeeded())
	{
		ptr_TileMesh->SetStaticMesh(meshFinder.Object);
		ptr_TileMesh->SetRelativeLocation(FVector(0, 0, 0));
	}

	/** defaults */
	TileHealth = 100;
	currentState = TileState::TILE_ACTIVE;
	TileTeamEnum = ETileTeam::VE_TileUA;

	isBaseTile = false;
	timer = 0.0f;
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

	ptr_TileMesh->BodyInstance.SetCollisionProfileName("BlockAll");
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (currentState == TileState::TILE_DESTROYED)
	//{
	//	timer += DeltaTime;

	//	if (timer > 5.0f)
	//		ReEnableTile();
	//}
}

//if tile has been hit by a cannon "remove tile from game"
void ATile::HasBeenHit()
{
	
	if (!isBaseTile)
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		currentState = TileState::TILE_DESTROYED;
		
	}
}

void ATile::ReEnableTile()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	currentState = TileState::TILE_ACTIVE;
}