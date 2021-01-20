// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Components//StaticMeshComponent.h"
#include "Bullet.generated.h"

UCLASS()
class MAINSOURCE_API ABullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABullet();
	void SetVelocity(FVector value) { vVelocity = value; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* pMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UBoxComponent* pBoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<ACharacter> charClass;

	UPROPERTY(EditAnywhere)
		FVector vVelocity;

private:
	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

};
