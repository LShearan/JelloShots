// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ConstructorHelpers.h"
#include "Cannon.h"
#include "Tile.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"
#include "Terminal.generated.h"

UCLASS()
class MAINSOURCE_API ATerminal : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATerminal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ShootCannon();
	void BuildPower();
	void RotateLeft();
	void RotateRight();
	void StopRotating();
	void ResetFire();
	void SpawnEmitter();

	UFUNCTION(BlueprintCallable)
	void Hit();
	
	void Unpossess();

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* pMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UParticleSystem* pShieldParticle;

	UPROPERTY(EditAnywhere)
	UParticleSystem* pGreenParticle;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* pYellowParticle;
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* pRedParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bActive;

	UPROPERTY(EditAnywhere)
	ACannon* pCannon;

	UPROPERTY(EditAnywhere)
		APawn* pPawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float power;
	
	UPROPERTY(EditAnywhere)
		float mult;

	UPROPERTY(EditAnywhere)
		bool build;

	UPROPERTY(EditAnywhere)
		bool collided;

	int playerId = 0;
	
	UPROPERTY(EditAnywhere)
		int teamID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int lives;

	UPROPERTY(EditAnywhere)
		bool bCanShoot;

	FTimerHandle tTimeBetweenShots;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float maxPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float minPower;

};
