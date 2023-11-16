// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Locked/Struct/DoorWayStruct.h"
#include "Locked/Struct/CardStruct.h"
#include "Locked/Enum/DirectionEnum.h"
#include "RoomTile.generated.h"

UCLASS()
class LOCKED_API ARoomTile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARoomTile();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		class UStaticMeshComponent* FloorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* WallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UBoxComponent* RoomDetectionCollider;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UNiagaraComponent* ParticleSystem;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		class UWidgetComponent* RoomCountWidget;

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

	bool bIsTreasureRoom = false;

	UPROPERTY(Replicated)
		bool bIsTreasureLooted = false;

	UPROPERTY(Replicated)
		bool FullyConnected = false;

	UPROPERTY(Replicated)
		bool bIsSmoked = false;

	UPROPERTY(EditAnywhere)
		int SmokeMaxDuration = 1;
	UPROPERTY(Replicated)
		int SmokeDuration = 0;

	int NextDuelTarget = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SpawnDoor() {};

	void CheckNeightbourRooms();

	void SetOppositeRoom(ETileDirection OriginDirection, ARoomTile* OriginRoom);

	UFUNCTION(Server, Reliable)
		void Server_RotateRoomPlacement();

	void CheckOtherRoomConnectionAvailability(ETileDirection OriginDirection);

	bool IsDoorWaysFullyFilled();

	FVector GetNextAvailableIdleSpot();

	void AddIdlePlayer(ALockedCharacter* PlayerCharacter);

	void RemovePlayerFromIdle(ALockedCharacter* PlayerCharacter);

	void PlaceIdlePlayerAtIdlePosition();

	void SetPlayerToDuelPosition(ALockedCharacter* PlayerCharacter, bool IsAttacker);

	virtual void AddPendingDroppedItem(FItemData NewDroppedItem);
	void FillDroppedItemWithPendingDrop();

	virtual bool RoomEvent(class APlayerControlPawn* Player);

	UFUNCTION(NetMultiCast, Reliable)
		void SmokeRoom(bool bState);

	UFUNCTION(Server, Reliable)
		void DestroyWall(bool bState, ETileDirection WallDirection);

	bool DuelTargetCheck(APlayerControlPawn* Player);

	bool TreasureCheck(APlayerControlPawn* Player);

	UFUNCTION(Server, Reliable)
		void Server_PickupItemCheck(APlayerControlPawn* Player);

	UFUNCTION(Server, Reliable)
		void Server_UpdateRoomState();

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_UpdateWidgetCount(int UpdateCountNumber);

	void Server_PickedItemOnFloor(FItemData PickedItem);

	UFUNCTION(Server, Reliable)
		void Server_DestroyDropItem();

	bool IsOppositeRoomConnected(ETileDirection OriginDirection);

	FDoorWay* GetOppositeDoorWay(ETileDirection OriginDirection);

	static ETileDirection GetOppositeDirection(ETileDirection OriginDirection);
};


