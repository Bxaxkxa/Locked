// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyProject/Private/Struct/DoorWayStruct.h"
#include "MyProject/Enum/DirectionEnum.h"
#include "PlayerControlPawn.generated.h"

UENUM()
enum class EMovementInputState
{
	E_CharMovement,
	E_TileMovement,
	E_TilePlacement
};


UCLASS()
class MYPROJECT_API APlayerControlPawn : public APawn
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_OneParam(FTestingDelegate, ETileDirection);
	// Sets default values for this pawn's properties
	APlayerControlPawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
		float CameraFollowSpeed;

	UPROPERTY(EditAnywhere, Category = Template, meta = (AllowPrivateAccess = "true"))
		TSubclassOf<class ALockedCharacter> CharacterTemplate;

	ALockedCharacter* ControlledCharacter;

	UPROPERTY(EditAnywhere, Category = Template, meta = (AllowPrivateAccess = "true"))
	class ARandomGenRoom* RandomRoomGenDeck;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class ARoomTile* CurrentRoom;

	ARoomTile* DrawedRoomTile;

	void SpawnControlledCharacter();

	bool bStillInMove = false;

	EMovementInputState CurrentMovementInputState;

	AActor* FollowTarget;

	FTimerHandle MovementTimer;

	ETileDirection LastPlacedDirection;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void ManageMovement(ETileDirection MoveToDirection);

	void MoveCameraTo(ARoomTile* MoveToTile);
	void MoveCharacterTo(ARoomTile* MoveToTile);
	void MovePlacedTile(FDoorWay DoorWay, ETileDirection MoveToDirection);

	void ChangeCameraBehaviour();

	void DrawRoomTile();
	void PlaceRoomTile();
	void RotateRoomTile();
};
