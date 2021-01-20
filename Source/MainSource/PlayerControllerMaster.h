// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Components/StaticMeshComponent.h"
#include "Gun.h"
#include "Bullet.h"
#include "Terminal.h"
#include "MainSourceGameModeBase.h"
#include "GameManager.h"
#include "Components/BillboardComponent.h"
#include "PlayerControllerMaster.generated.h"

//struct FTeam;

UCLASS()
class MAINSOURCE_API APlayerControllerMaster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerControllerMaster(const FObjectInitializer &ObjectInitializer);

	/** Team Methods */
	//void SetTeam(FTeam newTeam) { team = newTeam; }
	//FTeam GetTeam() { return team; }

    // Respawn methods
	void DisablePlayer();
	void EnablePlayer();
	bool GetDead();
	void Die();
	void SetPlayerColor(FColor newColor);
	void SetGameManager(AGameManager* newGameManager) { GM = newGameManager; }
	void PlayerRespawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Movement
	void MoveXAxisL(float value);
	void MoveYAxisL(float value);
	void MoveXAxisR(float value);
	void MoveYAxisR(float value);
	void HandleRotation();
	void HandleMovement(float);
	void OnFire();
	void Dash();
	void ResetDash();
	void CommitDash();
	void EndDash();

	// Health methods
	void DamagePlayer(float _damageAmt);
	void GiveHealth(float _healthAmt);
	void CheckKillHeight();

	//Posses methods
	void PossessCannon();

	// Getters
	float GetHealth() { return m_playerHealth; }

	// Controller Index
	void SetControllerIndex(int index) { ControllerIndex = index; }

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MaxSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float DashPower;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float DashDelay;


	UPROPERTY(EditDefaultsOnly, Category = "Movement")
		float MovementDelay;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	class UCharacterMovementComponent* pCharacterMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AGun* pGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ATerminal* pTerminal;

	APawn* pPlayer;

	FTeam* pTeam;

	UPROPERTY(EditAnywhere)
	class UBillboardComponent* pBillboardComponent;


	UPROPERTY(VisibleAnywhere)
		int ControllerIndex;

private:

	bool bIsDead;
	bool bCanDisable;
	bool bCanRespawn;
	float m_respawnCountdown;

	bool bCanDash;

	float RightStickXInput;
	float RightStickYInput;

	FRotator PlayerLastRotation;

	float m_playerMaxHealth;
	float m_playerHealth;

	float m_playerDeathHeight;

	AGameManager* GM;

protected:

	FTimerHandle DashTimerHandle;
	FTimerHandle RespawnTimer;
	FTimerHandle MovementTimer;

public:

	// Respawn Location Array
	UPROPERTY(EditAnywhere, Category = "Spawn Locations")
		TArray<class ATargetPoint*> RespawnLocation;

	UPROPERTY(EditAnywhere, Category = PlayerController)
		bool bAutoManageActiveCameraTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bAtStation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bRolling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bUsing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bHit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bCanMove;
	
private:

	// Debugging method for testing aspects
	void Debugging();
};
