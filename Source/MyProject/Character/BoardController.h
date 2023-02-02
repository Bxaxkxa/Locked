// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyProject/Struct/DoorWayStruct.h"
#include "MyProject/Struct/DuelData.h"
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

		DECLARE_DELEGATE_OneParam(FDirectionMovementDelegate, ETileDirection);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UInventoryComponent* InventoryComp;

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

	UPROPERTY(Replicated, BlueprintReadOnly)
		bool bIsTheAttacker;

	UPROPERTY(BlueprintReadWrite, Replicated)
		bool bIsReadyToStart = false;

	UPROPERTY(BlueprintReadWrite, Replicated)
		class UPlayerActionWidget* ActionUIWidget;

	UPROPERTY(BlueprintReadWrite, Replicated)
		class UActionIndicatorLayout* ActionIndicatorWidget;

	UPROPERTY(BlueprintReadWrite, Replicated)
		class UDuelOption* DuelOptionWidget;

	UPROPERTY(BlueprintReadWrite, Replicated)
		class UDuelWidget* DuelWidget;

	UPROPERTY(BlueprintReadWrite, Replicated)
		class UPlayersHealthWidget* PlayerHealthInfo;

	FTimerHandle UIDelayTimerHandle;
	float UIDelayTime = 0.2f;

	int RemainingChallegeTimes = 1;
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

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_DisplayDuelOption(APlayerControlPawn* DuelTarget, bool IsPlayerTheAttacker);

	UFUNCTION(Client, Reliable)
		void Client_HideDuelOption();

	UFUNCTION(Client, Reliable)
		void Client_ChangeDuelState();

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void Server_TellGameModeToUpdateDuelState();

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void Server_StopDuel();

	UFUNCTION(Server, Reliable)
		void Server_StartDuel();

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void Server_QueuWeapon(FItemData WeaponData, int WeaponValue);

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void Server_UnQueuWeapon();

	UFUNCTION(Client, Reliable)
		void Client_UpdateDuelUI(const TArray<FDuelData>& UpdatedDuelData);

	UFUNCTION(Client, Reliable)
		void Client_DisplayDuelUI();

	UFUNCTION(Client, Reliable)
		void Client_HideDuelUI();

	UFUNCTION(Client, Reliable)
		void Client_PlayDuelAnimation(EDuelResult DuelResult);

	UFUNCTION(Server, Reliable)
		void Server_UpdateDuelDiceState(bool IsStillRolling, int32 DiceNumber);

	UFUNCTION(Client, Reliable)
		void Client_UpdateDuelDiceVisual(FDuelDiceData UpdatedDiceData);

	void DisplayDuelOption(APlayerControlPawn* DuelTarget, ABoardController* TargetController, bool IsPlayerTheAttacker);

	UFUNCTION(Client, Reliable, BlueprintCallable)
		void Client_ChangeIndicatorLayout(EActionLayout LayoutStyle);

	UFUNCTION(Server, Reliable)
		void Server_StartPlayerTurn();

	UFUNCTION(Server, Reliable)
		void Server_DuelDiceOutcome(EDuelResult DuelResult);

	UFUNCTION(Server, Reliable)
		void Server_ChangeCameraPerspective(class ALockedCharacter* CurrentTurnPlayerCharacter);

	UFUNCTION(Server, Reliable)
		void Server_EndTurn();
	UFUNCTION(Server, Reliable)
		void Server_ChangeTurn();

	UFUNCTION(BlueprintCallable, Server, Reliable)
		void Server_TakeDamage();

	UFUNCTION(Client, Reliable)
		void Client_UpdatePlayerHealthInfo(const TArray<int>& UpdatedHealth);

	UFUNCTION(Client, Reliable)
		void Server_UsedUpWeapon(FItemData WeaponData);

	void CheckRoomForDualTarget();

	FORCEINLINE bool IsPlayerReady() { return bIsReadyToStart; }
	FORCEINLINE bool IsItPlayersTurn() { return bIsInTurn; }
	FORCEINLINE bool IsPlayerTheDuelAttacker() { return bIsTheAttacker; }
	UFUNCTION(BlueprintCallable)
		FORCEINLINE int GetRemainingChallegeTimes() { return RemainingChallegeTimes; }
	UFUNCTION(BlueprintCallable)
		FORCEINLINE UDuelOption* GetDuelWidget() { return DuelOptionWidget; }
	ALockedCharacter* GetPlayerCharacter();

	//int GetAvailableMove();
};
