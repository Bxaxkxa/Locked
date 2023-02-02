// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyProject/Struct/DoorWayStruct.h"
#include "MyProject/Enum/DirectionEnum.h"
#include "MyProject/Enum/PlayerMovementState.h"
#include "PlayerControlPawn.generated.h"

UCLASS()
class MYPROJECT_API APlayerControlPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerControlPawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* RootScene;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"), Replicated)
		float CameraFollowSpeed;

	UPROPERTY(EditAnywhere, Category = Template, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class ALockedCharacter> CharacterTemplate;

	UPROPERTY(Replicated)
		ALockedCharacter* ControlledCharacter;

	UPROPERTY(EditAnywhere, Category = Template, meta = (AllowPrivateAccess = "true"), Replicated)
		class ARandomGenRoom* RandomRoomGenDeck;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
		class ARoomTile* CurrentRoom;

	UPROPERTY(Replicated)
		ARoomTile* DrawedRoomTile;

	UPROPERTY(Replicated)
		bool bStillInMove = false;

	UPROPERTY(Replicated)
		EMovementInputState CurrentMovementInputState;

	UPROPERTY(VisibleAnywhere, Replicated)
		AActor* FollowTarget;

	bool bIsPlayerTurn = false;

	ETileDirection LastPlacedDirection;

	int NextDuelTarget = 0;

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

	void MoveCameraTo(ARoomTile* MoveToTile);
	void MoveCharacterTo(ARoomTile* MoveToTile);
	UFUNCTION(Server, Reliable)
		void Server_MoveCharacterTo(ARoomTile* MoveToTile, ETileDirection MoveToDirection);
	void MovePlacedTile(FDoorWay DoorWay, ETileDirection MoveToDirection);
	UFUNCTION(Server, Reliable)
		void Server_MovePlacedTile(FDoorWay DoorWay, ETileDirection MoveToDirection);

	void ChangeCameraBehaviour(EMovementInputState NewInputState);
	void ChangeCameraPerspective(ALockedCharacter* CurrentTurnPlayerCharacter);

	void BackToActionMenu();

	void DrawRoomTile();
	void PlaceRoomTile();

	void RotateRoomTile();
	UFUNCTION(Server, Reliable)
		void Server_RotateRoomTile();

	void DuelCheck(ALockedCharacter* DuelTarget);
	UFUNCTION(BlueprintCallable)
		bool CheckRoomForDualTarget();

	void CheckMovePoint();

	void StartTurn();
	void EndTurn();

	FORCEINLINE ALockedCharacter* GetPlayerCharacter() { return ControlledCharacter; }
};
