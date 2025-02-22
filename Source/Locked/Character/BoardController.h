// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Locked/Struct/DoorWayStruct.h"
#include "Locked/Struct/DuelData.h"
#include "Locked/Enum/DirectionEnum.h"
#include "Locked/Enum/PlayerMovementState.h"
#include "Locked/Enum/ObtainItemMethod.h"
#include "Locked/Enum/ActionIndicatorEnum.h"
#include "BoardController.generated.h"

/**
 *
 */
DECLARE_DELEGATE_OneParam(FDirectionMovementDelegate, ETileDirection);
DECLARE_DELEGATE(FContinueFunction);

UCLASS()
class LOCKED_API ABoardController : public APlayerController
{
	GENERATED_BODY()


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), BlueprintGetter = "GetInventory")
		class UInventoryComponent* InventoryComp;

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;
	virtual void AcknowledgePossession(APawn* PossesedPawn) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		TSubclassOf<class UPlayerTurnDisplay> TurnDisplayWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		TSubclassOf<class UStealCardWidget> StealCardWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		TSubclassOf<class UInventoryWidget> InventoryWidgetClass;

	UPROPERTY(BlueprintReadOnly, Replicated)
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
		class UInventoryWidget* InventoryWidget;

	UPROPERTY(BlueprintReadWrite, Replicated)
		class UActionIndicatorLayout* ActionIndicatorWidget;

	UPROPERTY(BlueprintReadWrite, Replicated)
		class UDuelOption* DuelOptionWidget;

	UPROPERTY(BlueprintReadWrite, Replicated)
		class UDuelWidget* DuelWidget;

	UPROPERTY(BlueprintReadWrite, Replicated)
		class UObtainedItemUI* ObtainItemWidget;

	UPROPERTY(BlueprintReadWrite, Replicated)
		class UPlayersHealthWidget* PlayerHealthInfo;

	UPROPERTY(BlueprintReadWrite, Replicated)
		class UOverCapacityWidget* OverCapacityWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UPawDuplication* PawDuplicationWidget;

	UPROPERTY(BlueprintReadWrite, Replicated)
		class ULoadingScreen* LoadingScreen;

	UPROPERTY(BlueprintReadWrite, Replicated)
		class UTextMessageWidget* TextMessageWidget;

	UPROPERTY(BlueprintReadWrite)
		class UUserWidget* MovePointIndicator;

	UPROPERTY(BlueprintReadWrite, Replicated)
		class UOffScreenIndicatorWidget* OffScreenWidget;

	FTimerHandle UIDelayTimerHandle;
	float UIDelayTime = 0.2f;

	int RemainingChallegeTimes = 1;

public:
	FContinueFunction ContinuePlayerAction;
	UPROPERTY(BlueprintReadWrite)
		class UWidget* LastFocusedWidget;

	ABoardController();

	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* aPawn) override;
	virtual void OnUnPossess() override;

	UFUNCTION(Server, Reliable)
		void Server_ManageMovement(ETileDirection MoveToDirection);

	UFUNCTION(Server, Reliable)
		void Confirm();

	UFUNCTION(Client, Reliable)
		void Client_AddAllGUIToViewport();

	UFUNCTION(Client, Reliable)
		void Client_AddAllPlayerLocationInfo();

	UFUNCTION(Client, Reliable)
		void Client_ShowActionWidget(bool On);
	UFUNCTION(Client, Reliable)
		void Client_ShowIndicatorLayout(bool bShowWidget);

	void ShowActionWidget(bool On);

	UFUNCTION(BlueprintCallable)
		void Back();

	void ChangeToViewMap();

	UFUNCTION(Server, Reliable)
		void Server_ChangeCameraBehaviour(EMovementInputState NewInputState);

	void SetPlayerMovementStateToMoveState();

	UFUNCTION(Client, Reliable)
		void Client_SetControllerInputMode(bool bIsForGameOnly);

	UFUNCTION(Client, Reliable)
		void Client_RemoveInventoryWidget();
	UFUNCTION(Client, Reliable)
		void Client_ShowInventoryWidget();

	UFUNCTION(Server, Reliable)
		void Server_DrawRoomTile();
	UFUNCTION(Server, Reliable)
		void Server_PlaceRoomTile();
	UFUNCTION(Server, Reliable)
		void Server_RotateRoomTile();

	UFUNCTION(Client, Reliable)
		void Client_DisplayLoadingScreen(class ULoadingScreen* LoadingScreenWidget);

	UFUNCTION(Client, Reliable)
		void Client_RemoveLoadingScreen();

	UFUNCTION(Client, Reliable)
		void Client_ShowTextMessage(const FString& NewMessage);

	UFUNCTION(Client, Reliable)
		void Client_DisplayPlayerTurn(int Turn);

	UFUNCTION(NetMulticast, Reliable)
		void Multicast_DisplayDuelOption(APlayerControlPawn* DuelTarget, bool IsPlayerTheAttacker);

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void SetupHealthInfo();

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
		void Client_OpenPawDuplication(bool IsDuel);

	UFUNCTION(Client, Reliable)
		void Client_PlayDuelAnimation(EDuelResult DuelResult);

	UFUNCTION(Server, Reliable)
		void Server_UpdateDuelDiceState(bool IsStillRolling, int32 DiceNumber);

	UFUNCTION(Client, Reliable)
		void Client_UpdateDuelDiceVisual(FDuelDiceData UpdatedDiceData);

	void DisplayDuelOption(APlayerControlPawn* DuelTarget, ABoardController* TargetController, bool IsPlayerTheAttacker);

	UFUNCTION(BlueprintCallable, Client, Reliable)
		void Client_DisplayOverCapacityUI(EObtainItemMethod ObtainMethod);

	UFUNCTION(Client, Reliable, BlueprintCallable)
		void Client_ChangeIndicatorLayout(EActionLayout LayoutStyle);

	UFUNCTION(Client, Reliable)
		void Client_ShowMovePointIndicator(bool IsActive);

	UFUNCTION(Server, Reliable)
		void Server_StartPlayerTurn();

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void Server_RunDuelOutcome(EDuelResult DuelResult);

	UFUNCTION(Client, Reliable)
		void Client_StealOpponentInventory(const TArray<FItemData>& OpponentInventory);

	UFUNCTION(Server, Reliable)
		void Server_StealOpponentItem(FItemData OpponentInventory);

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void Server_PickItemOnFloor();

	UFUNCTION(Server, Reliable)
		void Server_RunPendingDuelOutcome();

	UFUNCTION(Server, Reliable)
		void Server_UseKeyOnMainRoom();

	UFUNCTION(Client, Reliable)
		void UsedUpRifle();

	UFUNCTION(Server, Reliable)
		void Server_ChangeCameraPerspective(class ALockedCharacter* CurrentTurnPlayerCharacter);

	UFUNCTION(Server, Reliable)
		void Server_EndTurn();
	UFUNCTION(Server, Reliable)
		void Server_ChangeTurn();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


	UFUNCTION(Server, Reliable)
		void Server_GameModeUpdateHealthInfo();

	UFUNCTION(Client, Reliable)
		void Client_UpdatePlayerHealthInfo(const TArray<int>& UpdatedHealth);

	UFUNCTION(Server, Reliable)
		void Server_GameModeUpdateInventoryInfo();

	UFUNCTION(Client, Reliable)
		void Client_UpdatePlayerInventoryInfo(const TArray<int>& UpdatedItemCount, const TArray<int>& UpdatedTreasureCount);

	UFUNCTION(Client, Reliable)
		void Client_SetPlayerHealthInfo(const TArray<FString>& PlayersName);

	UFUNCTION(Server, Reliable)
		void Server_UsedUpWeapon(FItemData WeaponData);

	UFUNCTION(Server, Reliable)
		void Server_TriggerTrapCard(FItemData TrapCard);

	UFUNCTION(BlueprintCallable)
		void CheckRoomForDualTarget();

	UFUNCTION(Server, Reliable)
		void DrawCardFromDeck();
	UFUNCTION(Server, Reliable)
	void DrawTreasureFromDeck();

	UFUNCTION(BlueprintCallable, Server, Reliable)
		void Server_ObtainItem(FItemData ObtainedItem, EObtainItemMethod ObtainMethod);
	UFUNCTION(Client, Reliable)
		void Client_DisplayObtainItem(FItemData ObtainedItem, EObtainItemMethod ObtainMethod);

	UFUNCTION(BlueprintCallable, Server, Reliable)
		void Server_ContinueActionAfterObtainingItem();

	void DropItemOnRoom(FItemData WeaponData);
	UFUNCTION(Server, Reliable)
		void DropAllItemOnRoom();

	FORCEINLINE bool IsPlayerReady() { return bIsReadyToStart; }
	FORCEINLINE bool IsItPlayersTurn() { return bIsInTurn; }
	FORCEINLINE bool IsPlayerTheDuelAttacker() { return bIsTheAttacker; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE int GetRemainingChallegeTimes() { return RemainingChallegeTimes; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE UDuelOption* GetDuelWidget() { return DuelOptionWidget; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE UInventoryComponent* GetInventory() { return InventoryComp; }
	UFUNCTION(BlueprintPure)
		ALockedCharacter* GetPlayerCharacter();
	UFUNCTION(BlueprintPure)
		APlayerControlPawn* GetBoardPlayerControl() { return BoardPlayer; }

	//int GetAvailableMove();
};
