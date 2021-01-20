// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/BillboardComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TeamSelectionManager.h"
#include "Engine/Texture2D.h"
#include "TeamSelectionCharacter.generated.h"


UENUM()
enum class EPlayerState : uint8
{
	VE_REDTEAM		UMETA(DisplayName = "RedTeam"),
	VE_DEFAULTTEAM	UMETA(DisplayName = "Default"),
	VE_BLUETEAM		UMETA(DisplayName = "BlueTeam")
};

UCLASS()
class MAINSOURCE_API ATeamSelectionCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATeamSelectionCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void XAction();
	void BAction();

	void SetVisibility(bool visible);
	void SetReady(bool ready);

	void MoveLeft();
	void MoveRight();
	void MoveAxis(float inputVal);

	/** Move for polish, stops moving too much on input */
	void ResetInputCanMove() { InputCanMove = true; }

	void SetColor(FColor newColor);
	FColor GetColor();

	void StartGame();

public:

	int GetTexIndicatorID();
	bool GetReady() { return PlayerReady; }
	int GetTeamID() { return TeamID; }
	void SetStart(bool newStart) { CanStart = newStart; }

	UFUNCTION(BlueprintImplementableEvent)
	void LoadLevelBP();

	int id;
public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	UPROPERTY(EditAnywhere)
		class UBillboardComponent* pBillboardComponent;

private:

	bool PlayerReady;
	bool PlayerVisible;

	/** Can the player move left or right */
	bool PlayerML;
	bool PlayerMR;

	/** Can the player input */
	bool InputCanMove;
	bool CanStart;

	float InputMoveTimer;

	/** The starter position of the player */
	FVector StartingPos;

	/** For the color of the pawn */
	FColor DefaultColor;

	ATeamSelectionManager* MainGameManager2;

	int TexIndicatorID;
	int TeamID;

	float MovementValue = 3000.0f;

protected:

	FTimerHandle InputHandle;
	EPlayerState PlayerTeamState;

};
