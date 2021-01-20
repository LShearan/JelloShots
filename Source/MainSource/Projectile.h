// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ConstructorHelpers.h"
#include "Tile.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/PhysicsEngine/RadialForceComponent.h"
#include "Projectile.generated.h"

UCLASS()
class MAINSOURCE_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* hitComp, AActor* OtherActor, UPrimitiveComponent* other, FVector NormalImpulse, const FHitResult& Hit);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetVelocity(FVector value) { vVelocity = value; }

	UStaticMeshComponent* pMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* MyComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<AActor> explosionClass;

	FVector vVelocity;
	FRotator vRotator;
};
