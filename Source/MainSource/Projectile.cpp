// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "Engine/StaticMesh.h"
#include "DrawDebugHelpers.h"
#include "PlayerControllerMaster.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootComponent"));
	//pMeshComponent->SetupAttachment(RootComponent);

		
	static ConstructorHelpers::FObjectFinder<UStaticMesh>meshFinder(TEXT("StaticMesh'/Game/Models/BulletProjectile.BulletProjectile'"));

	if (pMeshComponent && meshFinder.Succeeded())
	{
		pMeshComponent->SetStaticMesh(meshFinder.Object);
		pMeshComponent->SetCollisionProfileName("BlockAllDynamic");
	}

	MyComp = CreateDefaultSubobject<UBoxComponent>(TEXT("COMP"));
	MyComp->SetWorldScale3D(FVector(24.f, 24.f, 24.f));
	MyComp->SetSimulatePhysics(true);
	MyComp->SetNotifyRigidBodyCollision(true);
	MyComp->BodyInstance.SetCollisionProfileName("BlockAllDynamic");
	MyComp->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

	MyComp->SetupAttachment(RootComponent);
	pMeshComponent->SetupAttachment(MyComp);

	static ConstructorHelpers::FObjectFinder<UClass> explosionClassFinder(TEXT("Blueprint'/Game/BP_Explosion.BP_Explosion_C'"));

	explosionClass = explosionClassFinder.Object;

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(4.0f);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	vVelocity -= FVector(0, 0, 100) * DeltaTime;
	this->SetActorLocation(GetActorLocation() + vVelocity, true);
}

void AProjectile::OnHit(UPrimitiveComponent* hitComp, AActor* OtherActor,UPrimitiveComponent* other, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor)
	{
		if (OtherActor->IsA(ATile::StaticClass()))
		{

			ATile* tile(Cast<ATile>(OtherActor));
			if (tile)
			{
				const FVector location = GetActorLocation();
				const FRotator rotation = GetActorRotation();

				GetWorld()->SpawnActor(explosionClass, &location, &rotation);

				tile->HasBeenHit();
				this->Destroy();
			}
		}
		else if(OtherActor->IsA(APlayerControllerMaster::StaticClass()))
		{
			const FVector location = GetActorLocation();
			const FRotator rotation = GetActorRotation();

			GetWorld()->SpawnActor(explosionClass, &location, &rotation);
			this->Destroy();
		}
		else
		{
			this->Destroy();
		}
	}

}