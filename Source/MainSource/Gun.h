// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.h"
#include "Classes/Kismet/GameplayStatics.h"
#include "Classes/Sound/SoundCue.h"
#include "Engine/World.h"
#include "UObject/ConstructorHelpers.h"
#include "Gun.generated.h"

UCLASS()
class MAINSOURCE_API AGun : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	void Fire();


	UPROPERTY(VisibleAnywhere)
		int currentAmmo;

	UPROPERTY(VisibleAnywhere)
		int fireDelay = 0;

	const int ammoMax = 5;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* pMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AActor> bulletClass;
	
	USoundCue* pShootCue;

private:

	float TimeBetweenShotsS;
	float AmmoRefreshRate;
	bool  bCanRefresh;
	bool  bCanFire;

protected:

	FTimerHandle FireRateHandle;
	FTimerHandle AmmoRefreshHandle;

	void ResetFire();
	void ResetRefresh();
};