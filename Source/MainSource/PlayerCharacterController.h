// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Cannon.h"
#include "GameFramework/SpringArmComponent.h"

#include "PlayerCharacterController.generated.h"

/**
*
*/
UCLASS()
class MAINSOURCE_API APlayerCharacterController : public ADefaultPawn
{
	GENERATED_BODY()

public:

	APlayerCharacterController();

protected:

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveXAxisL(float value);

	void MoveYAxisL(float value);

	void MoveXAxisR(float value);

	void MoveYAxisR(float value);

	void HandleRotation();

	void SetPos(FVector newPos);

	void OnFire();

	void Dash();

	void ResetDash();

	float GetHealth();
	
	void SetHealth(float newHealth);

	float TakeDamage(
		float _damage,
		struct FDamageEvent const & DamageEvent,
		AController * EventInstigator,
		AActor * DamageCauser
	) override;

	void Die();

	// Cannon stuff
	void MoveCannonRight();
	void MoveCannonLeft();
	void StopCannon();
	void ShootCannon();
	void BuildCannonPower();

private:

	float RightStickXInput;

	float RightStickYInput;

public:

	/** The delay between the last dash and when the player can dash again */
	float DashDelay;

	/** The amount of power to be asserted onto the dash */
	float DashPower;

	/** Whether the player is able to dash */
	bool bCanDash;

protected:

	FTimerHandle DashTimerHandle;

private:

	FRotator PlayerLastRotation;

public:

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* pMeshComponent;
	UPROPERTY(VisibleAnywhere)
		USpringArmComponent* pSpringArm;
	UPROPERTY(VisibleAnywhere)
		UCameraComponent* pCamera;

	UPROPERTY(EditAnywhere)
		ACannon* pCannon;

private:
	UPROPERTY(EditAnywhere)
		float health;

	UPROPERTY(VisibleAnywhere)
		float power;

	UPROPERTY(VisibleAnywhere)
		bool build;
};
