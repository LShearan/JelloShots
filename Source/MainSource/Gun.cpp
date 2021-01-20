// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

// Sets default values
AGun::AGun()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set the Root Component
	pMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComponent"));
	pMeshComponent->SetupAttachment(RootComponent);

	// Find and use a static mesh assets
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshFinder(TEXT("StaticMesh'/Game/Assets/Models/Gun.Gun'"));
	if (pMeshComponent && meshFinder.Succeeded())
	{
		pMeshComponent->SetStaticMesh(meshFinder.Object);
		pMeshComponent->SetRelativeLocation(FVector(0, 0, 0));
		pMeshComponent->SetCollisionProfileName(TEXT("OverlapAll"));
	}


	static ConstructorHelpers::FObjectFinder<UClass> bulletClassFinder(TEXT("Blueprint'/Game/BP_Bullet.BP_Bullet_C'"));

	bulletClass = bulletClassFinder.Object;

	static ConstructorHelpers::FObjectFinder<USoundCue> pShootCueFinder(TEXT("SoundCue'/Game/SFX/Player_Shoot_Cue.Player_Shoot_Cue'"));
	if (pShootCueFinder.Succeeded()) 
	{
		pShootCue = pShootCueFinder.Object;
	}
	/** some nice defaults for y'all */
	TimeBetweenShotsS = 1.0f;
	AmmoRefreshRate = 3.0f;
	bCanRefresh = true;
	bCanFire = true;
	 
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();


	currentAmmo = ammoMax;

}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (currentAmmo < ammoMax && bCanRefresh)
	{
		bCanRefresh = false;
		currentAmmo++;
		GetWorldTimerManager().SetTimer(AmmoRefreshHandle, this, &AGun::ResetRefresh, AmmoRefreshRate, false);
	}
}

void AGun::Fire()
{

	if (bCanFire && currentAmmo > 0)
	{
		UGameplayStatics::PlaySound2D(this, pShootCue, 0.5, 1);
		// Spawn a Bullet actor in front of the ship.
		//ABullet* pBullet = GetWorld()->SpawnActor<ABullet>(Location, Rotation, SpawnInfo);

		FVector location = GetAttachParentActor()->GetActorLocation() + GetAttachParentActor()->GetActorForwardVector() * 1100.f;
		FRotator rotation = FRotator(0, 0, 0);
		FActorSpawnParameters SpawnInfo;

		ABullet* pBullet = GetWorld()->SpawnActor<ABullet>(bulletClass, location, rotation, SpawnInfo);


		// Setup the Bullet's velocity.
		pBullet->SetVelocity(GetAttachParentActor()->GetActorForwardVector() * 500);

		fireDelay = 100;
		--currentAmmo;

		bCanFire = false;

		// Set the timer for when the bullet can next fire, very nice
		GetWorldTimerManager().SetTimer(FireRateHandle, this, &AGun::ResetFire, TimeBetweenShotsS, false);
	}

}

void AGun::ResetFire()
{
	bCanFire = true;
}

void AGun::ResetRefresh()
{
	bCanRefresh = true;
	UE_LOG(LogTemp, Warning, TEXT("Ammo State: %d"), currentAmmo)
}