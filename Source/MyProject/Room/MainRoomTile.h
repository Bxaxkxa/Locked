// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RoomTile.h"
#include "MainRoomTile.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API AMainRoomTile : public ARoomTile
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMainRoomTile();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* LeftDoorWay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* RightDoorWay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* TopDoorWay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* DownDoorWay;

protected:
	virtual void BeginPlay() override;

public:
	virtual void SpawnDoor() override;

};
