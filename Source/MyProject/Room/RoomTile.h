// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyProject/Private/Struct/DoorWayStruct.h"
#include "MyProject/Enum/DirectionEnum.h"
#include "RoomTile.generated.h"

UCLASS()
class MYPROJECT_API ARoomTile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARoomTile();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UBoxComponent* RoomDetectionCollider;

	UPROPERTY(EditAnywhere)
		FDoorWay NeighbourRoom[ETileDirection::NUM];

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SpawnDoor() {};

	void CheckNeightbourRooms();

	void SetOppositeRoom(ETileDirection OriginDirection, ARoomTile* OriginRoom);

	void RotateRoomPlacement();

	void CheckConnectionAvailibility(ETileDirection OriginDirection);
};
