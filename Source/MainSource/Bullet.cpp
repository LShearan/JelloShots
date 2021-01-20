// Fill out your copyright notice in the Description page of Project Settings.

#include "Bullet.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMesh.h"
#include "PlayerControllerMaster.h"
#include "ConstructorHelpers.h"


// Sets default values
ABullet::ABullet()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set the Root Component
	pMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));

	// Find and use a static mesh asset.
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshFinder(TEXT("StaticMesh'/Game/Models/BulletProjectile.BulletProjectile'"));
	if (pMeshComponent && meshFinder.Succeeded())
	{
		pMeshComponent->SetStaticMesh(meshFinder.Object);
		pMeshComponent->SetRelativeLocation(FVector(0, 0, 0));
		pMeshComponent->SetNotifyRigidBodyCollision(true);
		pMeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	}

	pBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	pBoxComponent->SetNotifyRigidBodyCollision(true);
	pBoxComponent->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
	pBoxComponent->OnComponentHit.AddDynamic(this, &ABullet::OnHit);

	static ConstructorHelpers::FObjectFinder<UClass> charClassFinder(TEXT("Blueprint'/Game/BP_PlayerController.BP_PlayerController_C'"));

	charClass = charClassFinder.Object;



	pBoxComponent->SetupAttachment(RootComponent);
	pMeshComponent->SetupAttachment(pBoxComponent);

}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(10.0f);

}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	this->SetActorLocation(GetActorLocation() + vVelocity, true);
}


void ABullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	//	if (OtherActor)
//	{
//		auto* player = Cast<ACharacter>(OtherActor);
//
//		if (player)
//		{
//			FVector bulletLoc = GetActorLocation();
//			FVector playerLoc = player->GetActorLocation();
//
//			FVector launchVel = playerLoc - bulletLoc;
//
//			launchVel.Normalize();
//
//			launchVel *= 2000.f;
//
//			player->LaunchCharacter(launchVel, false, false);
//
//			this->Destroy();
//		}
//	}
}
