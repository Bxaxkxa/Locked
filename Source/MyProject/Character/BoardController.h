// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyProject/Private/Struct/DoorWayStruct.h"
#include "MyProject/Enum/DirectionEnum.h"
#include "BoardController.generated.h"

/**
 *
 */
UCLASS()
class MYPROJECT_API ABoardController : public APlayerController
{
	GENERATED_BODY()

		DECLARE_DELEGATE_OneParam(FTestingDelegate, ETileDirection);

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;
	virtual void AcknowledgePossession(APawn* PossesedPawn) override;

	UPROPERTY(Replicated)
		class APlayerControlPawn* BoardPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class ARoomTile* CurrentSelectedRoom;

	UPROPERTY(Replicated)
	bool bIsInTurn = false;

public:
	ABoardController();

	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

	UFUNCTION(Server, Reliable)
		void Server_ManageMovement(ETileDirection MoveToDirection);

	void ChangeCameraBehaviour();

	UFUNCTION(Server, Reliable)
		void Server_DrawRoomTile();
	UFUNCTION(Server, Reliable)
		void Server_PlaceRoomTile();
	UFUNCTION(Server, Reliable)
		void Server_RotateRoomTile();

	void StartPlayerTurn();

	UFUNCTION(Server,Reliable)
	void Server_EndTurn();
};
