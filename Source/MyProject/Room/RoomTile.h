// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyProject/Struct/DoorWayStruct.h"
#include "MyProject/Struct/CardStruct.h"
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

	UPROPERTY(EditAnywhere, Replicated)
		FDoorWay NeighbourRoom[ETileDirection::NUM];

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float PlayerIdleDistance;

	UPROPERTY(Replicated)
		FVector PlayerIdlePositions[4];

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float DuelDistance;

	UPROPERTY(Replicated)
		TArray<class ALockedCharacter*> IdlePlayers;

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Debug")
		TArray<FItemData> DroppedItem;

	UPROPERTY(VisibleAnywhere, Replicated, Category = "Debug")
		TArray<FItemData> PendingDroppedItem;

	bool FullyConnected = false;

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

	void CheckOtherRoomConnectionAvailibility(ETileDirection OriginDirection);

	bool IsDoorWaysFullyFilled();

	FVector GetNextAvailableIdleSpot();

	void AddIdlePlayer(ALockedCharacter* Player);

	void RemovePlayerFromIdle(ALockedCharacter* Player);

	void PlaceIdlePlayerAtIdlePosition();

	void SetPlayerToDuelPosition(ALockedCharacter* Player, bool IsAttacker);

	void AddPendingDroppedItem(FItemData NewDroppedItem);
	void FillDroppedItemWithPendingDrop();

	UFUNCTION(Server, Reliable)
		void Server_PickedItemOnFloor(FItemData PickedItem);

	FDoorWay* GetOppositeDoorWay(ETileDirection OriginDirection);
};


