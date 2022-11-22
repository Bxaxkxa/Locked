// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyProject/Struct/DoorWayStruct.h"
#include "MyProject/Enum/DirectionEnum.h"
#include "MyProject/Enum/PlayerMovementState.h"
#include "MyProject/Enum/ActionIndicatorEnum.h"
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		TSubclassOf<class UPlayerTurnDisplay> TurnDisplayWidgetClass;

	UPROPERTY(Replicated)
		class APlayerControlPawn* BoardPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		class ARoomTile* CurrentSelectedRoom;

	UPROPERTY(Replicated)
		bool bIsInTurn = false;

	UPROPERTY(BlueprintReadWrite, Replicated)
		bool bIsReadyToStart = false;

	UPROPERTY(BlueprintReadWrite, Replicated)
		class UPlayerActionWidget* ActionUIWidget;

	UPROPERTY(BlueprintReadWrite, Replicated)
		class UActionIndicatorLayout* ActionIndicatorWidget;

	FTimerHandle UIDelayTimerHandle;
	float UIDelayTime = 0.2f;

public:
	ABoardController();

	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

	UFUNCTION(Server, Reliable)
		void Server_ManageMovement(ETileDirection MoveToDirection);

	UFUNCTION(Client, Reliable)
		void Client_ShowActionWidget(bool On);
	UFUNCTION(Client, Reliable)
		void Client_ShowIndicatorLayout(bool bShowWidget);

	void ShowActionWidget(bool On);

	void Back();

	UFUNCTION(Server, Reliable)
		void Server_ChangeCameraBehaviour(EMovementInputState NewInputState);

	UFUNCTION(Server, Reliable)
		void Server_DrawRoomTile();
	UFUNCTION(Server, Reliable)
		void Server_PlaceRoomTile();
	UFUNCTION(Server, Reliable)
		void Server_RotateRoomTile();

	UFUNCTION(Client, Reliable)
		void Client_DisplayPlayerTurn(int Turn);

	UFUNCTION(Client, Reliable)
		void Client_ChangeIndicatorLayout(EActionLayout LayoutStyle);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_StartPlayerTurn();

	UFUNCTION(Server, Reliable)
		void Server_ChangeCameraPerspective(class ALockedCharacter* CurrentTurnPlayerCharacter);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_EndTurn();
	UFUNCTION(Server, Reliable)
		void Server_ChangeTurn();

	FORCEINLINE bool IsPlayerReady() { return bIsReadyToStart; }
	FORCEINLINE bool IsItPlayersTurn() { return bIsInTurn; }
	ALockedCharacter* GetPlayerCharacter();

	//int GetAvailableMove();
};
