// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardController.h"
#include "LockedCharacter.h"
#include "PlayerControlPawn.h"
#include "Locked/Components/InventoryComponent.h"
#include "Locked/GameMode/MyProjectGameMode.h"
#include "Locked/GameState/LockedGameState.h"
#include "Locked/Room/MainRoomTile.h"
#include "Locked/Widget/PlayerActionWidget.h"
#include "Locked/Widget/PlayerTurnDisplay.h"
#include "Locked/Widget/DuelOption.h"
#include "Locked/Widget/DuelWidget.h"
#include "Locked/Widget/ActionIndicatorLayout.h"
#include "Locked/Widget/OverCapacityWidget.h"
#include "Locked/Widget/PlayersHealthWidget.h"
#include "Locked/Widget/ObtainedItemUI.h"
#include "Locked/Widget/StealCardWidget.h"
#include "Locked/Widget/LoadingScreen.h"
#include "Locked/Widget/InventoryWidget.h"
#include "Locked/Widget/TextMessageWidget.h"
#include "Locked/Widget/PawDuplication.h"
#include "Locked/Room/RoomTile.h"
#include "State/LockedPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

ABoardController::ABoardController()
{
	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComp->SetIsReplicated(true);

	SetReplicates(true);
}

void ABoardController::BeginPlay()
{
	Super::BeginPlay();

	//SetInputMode(FInputModeGameAndUI());
	Client_SetControllerInputMode(false);

	ALockedPlayerState* State = GetPlayerState<ALockedPlayerState>();
	if (State)
	{
		State->OnMovementBehaviourChangeDelegate.BindUFunction(this, FName("ChangeCameraBehaviour"));
	}
}

void ABoardController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		InputComponent->BindAction<FDirectionMovementDelegate, ABoardController, ETileDirection>("Left", IE_Pressed, this, &ABoardController::Server_ManageMovement, ETileDirection::NextLeft);
		InputComponent->BindAction<FDirectionMovementDelegate, ABoardController, ETileDirection>("Right", IE_Pressed, this, &ABoardController::Server_ManageMovement, ETileDirection::NextRight);
		InputComponent->BindAction<FDirectionMovementDelegate, ABoardController, ETileDirection>("Up", IE_Pressed, this, &ABoardController::Server_ManageMovement, ETileDirection::NextUp);
		InputComponent->BindAction<FDirectionMovementDelegate, ABoardController, ETileDirection>("Down", IE_Pressed, this, &ABoardController::Server_ManageMovement, ETileDirection::NextDown);

		InputComponent->BindAction("ViewMap", IE_Pressed, this, &ABoardController::ChangeToViewMap);
		InputComponent->BindAction("Confirm", IE_Pressed, this, &ABoardController::Confirm);

		InputComponent->BindAction("DrawTile", IE_Pressed, this, &ABoardController::Server_DrawRoomTile);
		InputComponent->BindAction("RotateTile", IE_Pressed, this, &ABoardController::Server_RotateRoomTile);
		InputComponent->BindAction("Back", IE_Pressed, this, &ABoardController::Back);
	}
}

void ABoardController::AcknowledgePossession(APawn* PossesedPawn)
{
	Super::AcknowledgePossession(PossesedPawn);
}

void ABoardController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FString BoolStr = bIsInTurn ? "true" : "false";
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, "IsPlayerInTurn? " + BoolStr);
}

void ABoardController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	BoardPlayer = Cast<APlayerControlPawn>(aPawn);

}

void ABoardController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ABoardController::Confirm_Implementation()
{
	if (BoardPlayer && bIsInTurn)
	{
		BoardPlayer->Confirm();
	}
}

void ABoardController::Client_AddAllGUIToViewport_Implementation()
{
	ActionUIWidget->AddToViewport();
	ActionIndicatorWidget->AddToViewport(1);
	DuelOptionWidget->AddToViewport(1);
	DuelWidget->AddToViewport(1);
	ObtainItemWidget->AddToViewport(6);
	PlayerHealthInfo->AddToViewport();
	OverCapacityWidget->AddToViewport(3);
	MovePointIndicator->AddToViewport(2);
	TextMessageWidget->AddToViewport(4);
	PawDuplicationWidget->AddToViewport(3);

	SetupHealthInfo();
}

void ABoardController::Server_ManageMovement_Implementation(ETileDirection MoveToDirection)
{
	if (BoardPlayer && bIsInTurn)
	{
		BoardPlayer->ManageMovement(MoveToDirection);
	}
}

void ABoardController::Client_ShowActionWidget_Implementation(bool On)
{
	if (ActionUIWidget)
	{
		On ? ActionUIWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible) : ActionUIWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABoardController::ShowActionWidget(bool On)
{
	//Start drag quick time event after fade out finish
	FTimerDelegate TimerDel;
	TimerDel.BindUFunction(this, FName("Client_ShowActionWidget"), On);

	GetWorld()->GetTimerManager().SetTimer(UIDelayTimerHandle, TimerDel, UIDelayTime, false);
}

void ABoardController::Client_ShowIndicatorLayout_Implementation(bool On)
{
	if (ActionIndicatorWidget)
	{
		On ? ActionIndicatorWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible) : ActionIndicatorWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABoardController::Client_ChangeIndicatorLayout_Implementation(EActionLayout LayoutStyle)
{
	if (ActionIndicatorWidget)
	{
		if (!ActionIndicatorWidget->IsVisible())
		{
			ActionIndicatorWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		ActionIndicatorWidget->ChangeIndicatorLayout(LayoutStyle);
	}
}

void ABoardController::Back()
{
	if (BoardPlayer)
	{
		BoardPlayer->HandleBack();
	}
}

void ABoardController::ChangeToViewMap()
{
	Client_SetControllerInputMode(true);

	if (BoardPlayer->CurrentMovementInputState == EMovementInputState::E_CharMovement)
	{
		Server_ChangeCameraBehaviour(EMovementInputState::E_MoveTileMovement);
		Client_ChangeIndicatorLayout(EActionLayout::ViewMapAction);
	}
}

void ABoardController::SetPlayerMovementStateToMoveState()
{
	Server_ChangeCameraBehaviour(EMovementInputState::E_CharMovement);
	Client_ChangeIndicatorLayout(EActionLayout::MoveAction);
	Client_ShowMovePointIndicator(true);

	//UWidgetBlueprintLibrary::SetFocusToGameViewport();
	Client_SetControllerInputMode(true);
}

void ABoardController::Client_ShowInventoryWidget_Implementation()
{
	if (InventoryWidgetClass)
	{
		InventoryWidget = Cast<UInventoryWidget>(CreateWidget(this, InventoryWidgetClass));
		InventoryWidget->ParentWidget = ActionUIWidget;
		InventoryWidget->AddToViewport();
		InventoryWidget->SetFocus();

		Server_ChangeCameraBehaviour(EMovementInputState::E_Idle);
		Client_SetControllerInputMode(false);
	}
}

void ABoardController::Client_RemoveInventoryWidget_Implementation()
{
	if (InventoryWidget)
		InventoryWidget->RemoveFromParent();
}

void ABoardController::Client_SetControllerInputMode_Implementation(bool bIsForGameOnly)
{
	//SetInputMode(FInputModeGameAndUI());
	bIsForGameOnly ? SetInputMode(FInputModeGameOnly ()) : SetInputMode(FInputModeGameAndUI());
}

void ABoardController::Server_ChangeCameraBehaviour_Implementation(EMovementInputState NewInputState)
{
	if (BoardPlayer && bIsInTurn)
		BoardPlayer->ChangeCameraBehaviour(NewInputState);
}

void ABoardController::Client_RemoveLoadingScreen_Implementation()
{
	LoadingScreen->StartFadeOffAnimation();
}

void ABoardController::Client_ShowTextMessage_Implementation(const FString& NewMessage)
{
	TextMessageWidget->SetTextMessage(NewMessage);
	TextMessageWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void ABoardController::Client_DisplayPlayerTurn_Implementation(int Turn)
{
	if (TurnDisplayWidgetClass)
	{
		UPlayerTurnDisplay* TurnDisplay = Cast<UPlayerTurnDisplay>(CreateWidget(this, TurnDisplayWidgetClass));
		if (TurnDisplay)
		{
			TurnDisplay->AddToViewport(2);
			TurnDisplay->PlayShowTurnAnimation(Turn);
		}
	}
}

void ABoardController::SetupHealthInfo_Implementation()
{
	UWorld* World = GetWorld();

	AGameModeBase* GameMode = World->GetAuthGameMode();

	TArray<FString> PlayersName;
	for (int32 i = 0; i < GameMode->GetNumPlayers(); i++)
	{
		APlayerState* GlobalPlayerState = UGameplayStatics::GetPlayerState(World, i);

		PlayersName.Add(GlobalPlayerState->GetPlayerName());
	}

	Client_SetPlayerHealthInfo(PlayersName);
}

void ABoardController::Multicast_DisplayDuelOption_Implementation(APlayerControlPawn* DuelTarget, bool IsPlayerTheAttacker)
{
	Server_ChangeCameraBehaviour(EMovementInputState::E_Idle);
	Client_SetControllerInputMode(false);

	if (UKismetSystemLibrary::IsServer(GetWorld()))
	{
		bIsTheAttacker = IsPlayerTheAttacker;
	}

	if (DuelOptionWidget)
	{
		DuelOptionWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		DuelOptionWidget->SetDuelTarget(DuelTarget);
	}
}

void ABoardController::Client_HideDuelOption_Implementation()
{
	DuelOptionWidget->SetVisibility(ESlateVisibility::Hidden);
}

void ABoardController::Client_ChangeDuelState_Implementation()
{
	DuelOptionWidget->PlayToOptionAnimation();
}

void ABoardController::Server_TellGameModeToUpdateDuelState_Implementation()
{
	AMyProjectGameMode* GameMode = Cast<AMyProjectGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->SwitchDuelState();
}

void ABoardController::Server_StopDuel_Implementation()
{
	AMyProjectGameMode* GameMode = Cast<AMyProjectGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->StopDuel(true);
}

float ABoardController::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ALockedPlayerState* State = GetPlayerState<ALockedPlayerState>();
	if (State)
	{
		if (!State->TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser))
		{
			BoardPlayer->ControlledCharacter->Died();

			DropAllItemOnRoom();
			Server_EndTurn();
		}
	}

	Server_GameModeUpdateHealthInfo();

	return State->Health;
}

void ABoardController::Server_GameModeUpdateInventoryInfo_Implementation()
{
	AMyProjectGameMode* GameMode = Cast<AMyProjectGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->UpdatePlayerInventoryInfo();
}

void ABoardController::Client_UpdatePlayerInventoryInfo_Implementation(const TArray<int>& UpdatedItemCount, const TArray<int>& UpdatedTreasureCount)
{
	PlayerHealthInfo->UpdatePlayerInventory(UpdatedItemCount, UpdatedTreasureCount);
}

void ABoardController::Server_GameModeUpdateHealthInfo_Implementation()
{
	AMyProjectGameMode* GameMode = Cast<AMyProjectGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->UpdatePlayerHealthInfo();
}

void ABoardController::Client_SetPlayerHealthInfo_Implementation(const TArray<FString>& PlayersName)
{
	PlayerHealthInfo->SetupPlayerInfo(PlayersName);
}

void ABoardController::Client_UpdatePlayerHealthInfo_Implementation(const TArray<int>& UpdatedHealth)
{
	PlayerHealthInfo->UpdatePlayerHealth(UpdatedHealth);
}

void ABoardController::Server_UsedUpWeapon_Implementation(FItemData WeaponData)
{
	InventoryComp->Server_RemoveItemFromInventory(WeaponData);

	if (!WeaponData.bIsTreasure)
	{
		ALockedGameState* GameState = GetWorld()->GetGameState<ALockedGameState>();
		GameState->Server_PutItemToPile(WeaponData);
	}
}

void ABoardController::Server_TriggerTrapCard_Implementation(FItemData TrapCard)
{
	if (ContinuePlayerAction.IsBound())
		ContinuePlayerAction.Unbind();

	ALockedGameState* GameState = GetWorld()->GetGameState<ALockedGameState>();
	GameState->Server_PutItemToPile(TrapCard);

	switch (TrapCard.ItemName)
	{
	case EItemList::SpikeTrap:
		TakeDamage(2.0f, FDamageEvent(), nullptr, nullptr);
		return;
	case EItemList::ArrowTrap:
		if (!TakeDamage(1.0f, FDamageEvent(), nullptr, nullptr))
			return;
		break;
	case EItemList::NetTrap:
		Server_EndTurn();
		return;
	case EItemList::BearTrap:
		if (TakeDamage(1.0f, FDamageEvent(), nullptr, nullptr))
			Server_EndTurn();
		return;
	default:
		break;
	}

	if (!BoardPlayer->PickupItemOnCurrentRoom())
	{
		CheckRoomForDualTarget();
	}
}

void ABoardController::CheckRoomForDualTarget()
{
	if (ContinuePlayerAction.IsBound())
		ContinuePlayerAction.Unbind();

	BoardPlayer->Server_UpdateCurrentRoomDropItemData();

	if (!BoardPlayer->CheckRoomForDualTarget())
	{
		BoardPlayer->bStillInMove = false;
		SetPlayerMovementStateToMoveState();
		BoardPlayer->CheckMovePoint();
	}
}

void ABoardController::DrawCardFromDeck_Implementation()
{
	if (Cast<AMainRoomTile>(BoardPlayer->CurrentRoom))
	{
		Server_EndTurn();
		return;
	}

	ALockedGameState* GameState = GetWorld()->GetGameState<ALockedGameState>();

	FItemData DrawedItem;
	if (!GameState->DrawItem(DrawedItem))
	{
		Server_EndTurn();
		return;
	}

	Server_ObtainItem(DrawedItem, EObtainItemMethod::EndTurn);
}

void ABoardController::DrawTreasureFromDeck_Implementation()
{
	ALockedGameState* GameState = GetWorld()->GetGameState<ALockedGameState>();

	FItemData DrawedItem;
	if (!GameState->DrawTreasure(DrawedItem))
	{
		return;
	}

	Server_ObtainItem(DrawedItem, EObtainItemMethod::PickUp);
}

void ABoardController::Server_ObtainItem_Implementation(FItemData ObtainedItem, EObtainItemMethod ObtainMethod)
{
	Client_DisplayObtainItem(ObtainedItem, ObtainMethod);

	//if (ContinuePlayerAction.IsBound())
	//	ContinuePlayerAction.Unbind();

	switch (ObtainMethod)
	{
	case EObtainItemMethod::EndTurn:
		ContinuePlayerAction.BindUFunction(this, "Server_EndTurn");
		break;
	case EObtainItemMethod::PickUp:
		if (ObtainedItem.ItemType == EItemType::Trap)
		{
			ContinuePlayerAction.BindUFunction(this, "Server_TriggerTrapCard", ObtainedItem);
			return;
		}
		ContinuePlayerAction.BindUFunction(this, "Server_PickItemOnFloor");
		break;
	case EObtainItemMethod::Steal:
		//If statement if it is the attacker
		if (bIsTheAttacker)
		{
			ContinuePlayerAction.BindUFunction(this, "CheckRoomForDualTarget");
		}
		break;
	}

	InventoryComp->AddItemToInventory(ObtainedItem);
}

void ABoardController::Client_DisplayObtainItem_Implementation(FItemData ObtainedItem, EObtainItemMethod ObtainMethod)
{
	ObtainItemWidget->SetVisibility(ESlateVisibility::Visible);
	ObtainItemWidget->ObtainItem(ObtainedItem, ObtainMethod);
	ObtainItemWidget->SetFocus();
}

void ABoardController::Client_DisplayLoadingScreen_Implementation(ULoadingScreen* LoadingScreenWidget)
{
	if (LoadingScreenWidget)
	{
		LoadingScreenWidget->AddToViewport(5);
		LoadingScreenWidget->StartFadeOffAnimation();
	}
}

void ABoardController::Server_ContinueActionAfterObtainingItem_Implementation()
{
	bool bSuccess = ContinuePlayerAction.ExecuteIfBound();
}

void ABoardController::DropItemOnRoom(FItemData WeaponData)
{
	BoardPlayer->DropItemOnCurrentRoom(WeaponData);
}

void ABoardController::DropAllItemOnRoom_Implementation()
{
	InventoryComp->DropAllItem();
}

void ABoardController::Server_StartDuel_Implementation()
{
	RemainingChallegeTimes -= bIsTheAttacker;

	AMyProjectGameMode* GameMode = Cast<AMyProjectGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->StartDuel();
}

void ABoardController::Client_DisplayDuelUI_Implementation()
{
	if (DuelWidget)
	{
		DuelWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void ABoardController::Client_HideDuelUI_Implementation()
{
	if (DuelWidget)
	{
		DuelWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABoardController::Server_QueuWeapon_Implementation(FItemData WeaponData, int WeaponValue)
{
	FDuelData PlayerDuelData = FDuelData(WeaponData, bIsTheAttacker, WeaponValue);

	AMyProjectGameMode* GameMode = Cast<AMyProjectGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->QueuUpWeapon(PlayerDuelData);
}

void ABoardController::Server_UnQueuWeapon_Implementation()
{
	AMyProjectGameMode* GameMode = Cast<AMyProjectGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->UnQueuUpWeapon(bIsTheAttacker);
}

void ABoardController::Client_OpenPawDuplication_Implementation(bool IsDuel)
{
	PawDuplicationWidget->SetIsForDuel(IsDuel);

	PawDuplicationWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (IsDuel)
	{
		PawDuplicationWidget->SourceWidget = DuelWidget;
		return;
	}
	PawDuplicationWidget->SourceWidget = InventoryWidget;
}

void ABoardController::Client_UpdateDuelUI_Implementation(const TArray<FDuelData>& UpdatedDuelData)
{
	DuelWidget->UpdateDuelData(UpdatedDuelData);
}

void ABoardController::Client_PlayDuelAnimation_Implementation(EDuelResult DuelResult)
{
	DuelWidget->PlayDuelAnimation(DuelResult);
}

void ABoardController::Server_UpdateDuelDiceState_Implementation(bool IsStillRolling, int32 DiceNumber)
{
	AMyProjectGameMode* GameMode = Cast<AMyProjectGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		bIsTheAttacker ? GameMode->UpdateAttackerDuelDiceData(IsStillRolling, DiceNumber) : GameMode->UpdateDefenderDuelDiceData(IsStillRolling, DiceNumber);
	}
}

void ABoardController::Client_UpdateDuelDiceVisual_Implementation(FDuelDiceData UpdatedDiceData)
{
	DuelWidget->UpdateDiceRoll(UpdatedDiceData);
}

void ABoardController::DisplayDuelOption(APlayerControlPawn* DuelTarget, ABoardController* TargetController, bool IsPlayerTheAttacker)
{
	////Start drag quick time event after fade out finish
	//FTimerDelegate TimerDel;
	//TimerDel.BindUFunction(this, FName("Client_DisplayDuelOption"), DuelTarget, TargetController, IsPlayerTheAttacker);

	//GetWorld()->GetTimerManager().SetTimer(UIDelayTimerHandle, TimerDel, UIDelayTime, false);
}

void ABoardController::Client_ShowMovePointIndicator_Implementation(bool IsActive)
{
	IsActive ? MovePointIndicator->SetVisibility(ESlateVisibility::SelfHitTestInvisible) : MovePointIndicator->SetVisibility(ESlateVisibility::Hidden);
}

void ABoardController::Client_DisplayOverCapacityUI_Implementation(EObtainItemMethod ObtainMethod)
{
	Client_SetControllerInputMode(false);
	Server_ChangeCameraBehaviour(EMovementInputState::E_Idle);
	OverCapacityWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	OverCapacityWidget->PendingObtainMethod = ObtainMethod;
}

void ABoardController::Client_StealOpponentInventory_Implementation(const TArray<FItemData>& OpponentInventory)
{
	UStealCardWidget* TurnDisplay = Cast<UStealCardWidget>(CreateWidget(this, StealCardWidgetClass));
	if (TurnDisplay)
	{
		TurnDisplay->AddToViewport(3);
		TurnDisplay->ShuffleItemCardPosition(OpponentInventory);
	}
}

void ABoardController::Server_StealOpponentItem_Implementation(FItemData OpponentInventory)
{
	Server_ObtainItem(OpponentInventory, EObtainItemMethod::Steal);

	AMyProjectGameMode* GameMode = Cast<AMyProjectGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->StealOpponentCard(OpponentInventory);
		ContinuePlayerAction.BindUFunction(this, "Server_RunPendingDuelOutcome");
	}
}

void ABoardController::Server_RunDuelOutcome_Implementation(EDuelResult DuelResult)
{
	AMyProjectGameMode* GameMode = Cast<AMyProjectGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->StartPlayerStealCard(DuelResult);
	}
}

void ABoardController::Server_RunPendingDuelOutcome_Implementation()
{
	AMyProjectGameMode* GameMode = Cast<AMyProjectGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->RunPendingDuelResult();
	}
}

void ABoardController::Server_PickItemOnFloor_Implementation()
{
	if (BoardPlayer->CurrentRoom->DroppedItem.Num() == 0)
	{
		CheckRoomForDualTarget();
		return;
	}

	BoardPlayer->PickupItemOnCurrentRoom();
}

void ABoardController::Server_UseKeyOnMainRoom_Implementation()
{
	BoardPlayer->UseKeyAtMainRoom();
}

void ABoardController::UsedUpRifle_Implementation()
{
	InventoryComp->UsedUpRifle();
}

void ABoardController::Server_StartPlayerTurn_Implementation()
{
	bIsInTurn = true;

	Client_SetControllerInputMode(false);

	BoardPlayer->StartTurn();

	RemainingChallegeTimes = 1;
}

void ABoardController::Server_ChangeCameraPerspective_Implementation(ALockedCharacter* CurrentTurnPlayerCharacter)
{
	if (BoardPlayer)
	{
		BoardPlayer->ChangeCameraPerspective(CurrentTurnPlayerCharacter);
	}
}

void ABoardController::Server_EndTurn_Implementation()
{
	//FString Message = Reason;
	//GEngine->AddOnScreenDebugMessage(10, 8.0f, FColor::Red, "Reason: " + Message);

	if (!bIsInTurn)
		return;

	//Server_ObtainItem();
	if (ContinuePlayerAction.IsBound())
		ContinuePlayerAction.Unbind();

	BoardPlayer->Server_UpdateCurrentRoomDropItemData();

	Client_SetControllerInputMode(true);

	bIsInTurn = false;

	Server_ChangeTurn();

	Client_ShowActionWidget(false);
	Client_ShowMovePointIndicator(false);
}

void ABoardController::Server_ChangeTurn_Implementation()
{
	BoardPlayer->EndTurn();

	AMyProjectGameMode* GameMode = (AMyProjectGameMode*)GetWorld()->GetAuthGameMode();
	GameMode->ShowNextTurnDisplay();
	GameMode->NextPlayerTurn();
}

void ABoardController::Server_DrawRoomTile_Implementation()
{
	if (BoardPlayer && bIsInTurn)
	{
		BoardPlayer->DrawRoomTile();
	}
}

void ABoardController::Server_PlaceRoomTile_Implementation()
{
	if (BoardPlayer && bIsInTurn)
	{
		BoardPlayer->PlaceRoomTile();
	}
}

void ABoardController::Server_RotateRoomTile_Implementation()
{
	if (BoardPlayer && bIsInTurn)
	{
		BoardPlayer->RotateRoomTile();
	}
}

ALockedCharacter* ABoardController::GetPlayerCharacter()
{
	return BoardPlayer->GetPlayerCharacter();
}

void ABoardController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABoardController, BoardPlayer);
	DOREPLIFETIME(ABoardController, bIsInTurn);
	DOREPLIFETIME(ABoardController, ActionUIWidget);
	DOREPLIFETIME(ABoardController, DuelOptionWidget);
	DOREPLIFETIME(ABoardController, DuelWidget);
	DOREPLIFETIME(ABoardController, ObtainItemWidget);
	DOREPLIFETIME(ABoardController, OverCapacityWidget);
	DOREPLIFETIME(ABoardController, PlayerHealthInfo);
	DOREPLIFETIME(ABoardController, ActionIndicatorWidget);
	DOREPLIFETIME(ABoardController, TurnDisplayWidgetClass);
	DOREPLIFETIME(ABoardController, bIsReadyToStart);
	DOREPLIFETIME(ABoardController, bIsTheAttacker);
}
