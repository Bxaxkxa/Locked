// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RoomTile.generated.h"

USTRUCT(BlueprintType)
struct FDoorWay
{
	GENERATED_BODY();
public:
	/*UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		ARoomTile* NextRoom;*/
};

UCLASS()
class MYPROJECT_API ARoomTile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoomTile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<FString, ARoomTile*> NeighbourRoom;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SpawnDoor() {};
};
