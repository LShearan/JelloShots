// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConstructorHelpers.h"
#include "Projectile.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Classes/Kismet/GameplayStatics.h"
#include "Classes/Sound/SoundCue.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Components/InputComponent.h"
#include "Components/BillboardComponent.h"
#include "Cannon.generated.h"

UCLASS()
class MAINSOURCE_API ACannon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACannon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void FireAction(float);

	void RotateRight();
	void RotateLeft();
	void StopRotating();

	void SetYawSpeed(float value) { YawSpeed = value; }

	float YawSpeed, MaxYaw, MinYaw;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* pMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBillboardComponent* pBillboardComponent;
	
	UPROPERTY(EditAnywhere)
		float offset;

	UPROPERTY(EditAnywhere)
		float testPower;

	USoundCue* pCannonFireCue;

protected:

	FTimerHandle FireRateHandle;
	void ResetCanFire();
	bool bCanFire;
	float fireRefreshTimer;
	float minPower;
	float maxPower;
};
