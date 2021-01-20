// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/Texture2D.h"
#include "TeamSelectionManager.generated.h"

UCLASS()
class MAINSOURCE_API ATeamSelectionManager : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATeamSelectionManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(EditAnywhere)
		TArray<UTexture2D*> PlayerIndicators;

};
