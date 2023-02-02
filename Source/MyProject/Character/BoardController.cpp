// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardController.h"
#include "LockedCharacter.h"
#include "PlayerControlPawn.h"
#include "MyProject/Components/InventoryComponent.h"
#include "MyProject/GameMode/MyProjectGameMode.h"
#include "MyProject/GameState/LockedGameState.h"
#include "MyProject/Widget/PlayerActionWidget.h"
#include "MyProject/Widget/PlayerTurnDisplay.h"
#include "MyProject/Widget/DuelOption.h"
#include "MyProject/Widget/DuelWidget.h"
#include "MyProject/Widget/ActionIndicatorLayout.h"
#include "MyProject/Widget/PlayersHealthWidget.h"
#include "State/LockedPlayerState.h"
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

	SetInputMode(FInputModeGameAndUI());

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

		InputComponent->BindAction("PlaceTile", IE_Pressed, this, &ABoardController::Server_PlaceRoomTile);

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
		BoardPlayer->BackToActionMenu();
	}
}

void ABoardController::Server_ChangeCameraBehaviour_Implementation(EMovementInputState NewInputState)
{
	if (BoardPlayer && bIsInTurn)
	{
		BoardPlayer->ChangeCameraBehaviour(NewInputState);
	}
}

void ABoardController::Client_DisplayPlayerTurn_Implementation(int Turn)
{
	if (TurnDisplayWidgetClass)
	{
		UPlayerTurnDisplay* TurnDisplay = Cast<UPlayerTurnDisplay>(CreateWidget(this, TurnDisplayWidgetClass));
		if (TurnDisplay)
		{
			TurnDisplay->AddToViewport();
			TurnDisplay->PlayShowTurnAnimation(Turn);
		}
	}
}

void ABoardController::Multicast_DisplayDuelOption_Implementation(APlayerControlPawn* DuelTarget, bool IsPlayerTheAttacker)
{
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

void ABoardController::Server_TakeDamage_Implementation()
{
	ALockedPlayerState* State = GetPlayerState<ALockedPlayerState>();
	if (State)
	{
		State->TakeDamage();

		if (!State->Health)
		{
			GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Red, "Player Died");
			//TODO: Death
		}
	}
}

void ABoardController::Client_UpdatePlayerHealthInfo_Implementation(const TArray<int>& UpdatedHealth)
{
	PlayerHealthInfo->UpdatePlayerHealth(UpdatedHealth);
}

void ABoardController::Server_UsedUpWeapon_Implementation(FItemData WeaponData)
{
	InventoryComp->RemoveItemFromInventory(WeaponData);
	InventoryComp->RemoveItemFromInventory(WeaponData);
}

void ABoardController::CheckRoomForDualTarget()
{
	BoardPlayer->CheckRoomForDualTarget();
}

void ABoardController::Server_StartDuel_Implementation()
{
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

void ABoardController::Server_DuelDiceOutcome_Implementation(EDuelResult DuelResult)
{
	AMyProjectGameMode* GameMode = Cast<AMyProjectGameMode>(GetWorld()->GetAuthGameMode());

	switch (DuelResult)
	{
	case EDuelResult::AttackerWin:
		if (!bIsTheAttacker)
		{
			Server_TakeDamage();
			GameMode->StopDuel(true);
		}
		break;
	case EDuelResult::DefenderWin:
		if (bIsTheAttacker)
		{
			Server_TakeDamage();
			GameMode->StopDuel(false);
		}
		break;
	case EDuelResult::Draw:
		GameMode->StopDuel(true);
		break;
	default:
		break;
	}
}

void ABoardController::Server_StartPlayerTurn_Implementation()
{
	bIsInTurn = true;

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
	ALockedGameState* GameState = GetWorld()->GetGameState<ALockedGameState>();
	InventoryComp->AddItemToInventory(GameState->DrawItem());

	Server_ChangeTurn();

	Client_ShowActionWidget(false);
}

void ABoardController::Server_ChangeTurn_Implementation()
{
	if (bIsInTurn)
	{
		bIsInTurn = false;

		BoardPlayer->EndTurn();

		AMyProjectGameMode* GameMode = (AMyProjectGameMode*)GetWorld()->GetAuthGameMode();
		GameMode->ShowNextTurnDisplay();
		GameMode->NextPlayerTurn();
	}
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
	DOREPLIFETIME(ABoardController, PlayerHealthInfo);
	DOREPLIFETIME(ABoardController, ActionIndicatorWidget);
	DOREPLIFETIME(ABoardController, TurnDisplayWidgetClass);
	DOREPLIFETIME(ABoardController, bIsReadyToStart);
	DOREPLIFETIME(ABoardController, bIsTheAttacker);
}
