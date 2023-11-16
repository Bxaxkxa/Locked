// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Locked/Struct/DoorWayStruct.h"
#include "Locked/Struct/CardStruct.h"
#include "Locked/Enum/DirectionEnum.h"
#include "Locked/Enum/PlayerMovementState.h"
#include "PlayerControlPawn.generated.h"

UCLASS()
class LOCKED_API APlayerControlPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerControlPawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* RootScene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class ULockedFollowCamera* FollowCamera;

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"), Replicated)
		float CameraFollowSpeed;

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
		FTransform CameraFollowAngle;

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
		FTransform CameraViewMapAngle;

	UPROPERTY(EditAnywhere, Category = Template, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class ALockedCharacter> CharacterTemplate;

	UPROPERTY(Replicated)
		ALockedCharacter* ControlledCharacter;

	UPROPERTY(EditAnywhere, Category = Template, meta = (AllowPrivateAccess = "true"), Replicated)
		class ARandomGenRoom* RandomRoomGenDeck;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
		class ARoomTile* CurrentRoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
		class ARoomTile* ViewMapTarget;

	UPROPERTY(Replicated)
		ARoomTile* DrawedRoomTile;

	UPROPERTY(Replicated)
		bool bStillInMove = false;

	UPROPERTY(Replicated, VisibleAnywhere)
		EMovementInputState CurrentMovementInputState;

	UPROPERTY(VisibleAnywhere, Replicated)
		AActor* FollowTarget;

	bool bIsPlayerTurn = false;

	UPROPERTY(BlueprintReadOnly, Replicated)
		bool bIsOutOfMove = false;

	ETileDirection LastPlacedDirection;

	UPROPERTY(Replicated)
		TArray<ALockedCharacter*> RifleTargets;
	ALockedCharacter* CurrentRifleTargets;
	int CurrentRifleTargetsIndex;

	int NextDuelTarget = 0;

	ETileDirection CurrentHammerDirection;
	FItemData UsedHammer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SpawnControlledCharacter();

	void ManageMovement(ETileDirection MoveToDirection);

	void Confirm();

	void MoveCameraTo(ARoomTile* MoveToTile);
	void MoveCharacterTo(ARoomTile* MoveToTile);
	UFUNCTION(Server, Reliable)
		void Server_MoveCharacterTo(ARoomTile* MoveToTile, ETileDirection MoveToDirection);
	void MovePlacedTile(FDoorWay DoorWay, ETileDirection MoveToDirection);
	UFUNCTION(Server, Reliable)
		void Server_MovePlacedTile(FDoorWay DoorWay, ETileDirection MoveToDirection);

	UFUNCTION(Server, Reliable)
		void ChangeCameraBehaviour(EMovementInputState NewInputState);
	UFUNCTION(Server, Reliable)
		void ChangeCameraPerspective(ALockedCharacter* CurrentTurnPlayerCharacter);

	void HandleBack();
	void BackToActionMenu();
	void BackToInventory();
	void BackFromRifleState();
	void BackFromHammerState();

	void DrawRoomTile();
	void PlaceRoomTile();

	void RotateRoomTile();
	UFUNCTION(Server, Reliable)
		void Server_RotateRoomTile();

	UFUNCTION(Server, Reliable)
		void Server_CheckRifleTargetAvailability();
	void ChangeToRifleStance();
	UFUNCTION(Server, Reliable)
		void NextRifleTarget();
	UFUNCTION(Server, Reliable)
		void PrevRifleTarget();
	UFUNCTION(Server, Reliable)
		void ShootRifle();

	UFUNCTION(Server, Reliable)
		void Server_CheckWallForHammer(FItemData ItemCard);
	void ChangeToHammerStance(ETileDirection StartingDir);

	UFUNCTION(Server, Reliable)
		void DestroyWall();

	UFUNCTION(Server, Reliable)
		void ChangeHammerDirection(ETileDirection NewDirection);

	UFUNCTION(Server, Reliable)
		void SmokeRoom();

	void DuelCheck(ALockedCharacter* DuelTarget);
	UFUNCTION(BlueprintCallable)
		bool CheckRoomForDualTarget();

	bool CheckTreasureRoom();

	bool RunRoomEvent();

	void DropItemOnCurrentRoom(FItemData ItemData);
	UFUNCTION(BlueprintCallable, Server, Reliable)
		void Server_UpdateCurrentRoomDropItemData();

	UFUNCTION(BlueprintCallable)
		bool PickupItemOnCurrentRoom();

	UFUNCTION(BlueprintCallable)
		void UseKeyAtMainRoom();

	UFUNCTION(BlueprintCallable, Server, Reliable)
		void CheckMoveAvailability();

	void CheckMovePoint();

	void StartTurn();
	void EndTurn();

	FORCEINLINE ALockedCharacter* GetPlayerCharacter() { return ControlledCharacter; }
};
