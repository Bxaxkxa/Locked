// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RandomGenRoom.generated.h"

class ARoomTile;

UCLASS()
class MYPROJECT_API ARandomGenRoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARandomGenRoom();

	uint8 NumberOfATile = 10;
	uint8 NumberOfBTile = 10;
	uint8 NumberOfCTile = 10;
	uint8 NumberOfDTile = 10;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ARoomTile> RoomTemplates[4];

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	ARoomTile* DrawTile();

};
