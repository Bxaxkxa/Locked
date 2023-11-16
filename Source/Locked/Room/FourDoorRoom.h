// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoomTile.h"
#include "FourDoorRoom.generated.h"

/**
 * 
 */
UCLASS()
class LOCKED_API AFourDoorRoom : public ARoomTile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFourDoorRoom();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* LeftDoorWay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* RightDoorWay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* TopDoorWay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* DownDoorWay;

public:
	virtual void SpawnDoor() override;

};
