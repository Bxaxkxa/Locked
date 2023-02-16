// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerActionWidget.h"
#include "MyProject/Character/BoardController.h"
#include "MyProject/Character/State/LockedPlayerState.h"
#include "MyProject/Widget/ActionIndicatorLayout.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void UPlayerActionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GetWorld()->GetTimerManager().SetTimer(UIDelayTimerHandle, this, &UPlayerActionWidget::SetupPlayerVariable, UIDelayTime, false);
	
	if (MoveActionButton->OnButtonClick.IsBound())
		return;

	MoveActionButton->OnButtonClick.AddDynamic(this, &UPlayerActionWidget::DelayToSwitchToCharacterMoveState);
	ItemButton->OnButtonClick.AddDynamic(this, &UPlayerActionWidget::SetItemAsLastFocusedButton);
	ViewMapButton->OnButtonClick.AddDynamic(this, &UPlayerActionWidget::SwitchToTileMoveState);
	EndTurnButton->OnButtonClick.AddDynamic(this, &UPlayerActionWidget::DelayToEndTurn);

	OnVisibilityChanged.AddDynamic(this, &UPlayerActionWidget::VisibilityChange);
}

void UPlayerActionWidget::SwitchToCharacterMoveState()
{
	if (PlayerOwner)
	{
		PlayerOwner->SetPlayerMovementStateToMoveState();
	}

	SetVisibility(ESlateVisibility::Hidden);
	LastFocusedButton = MoveActionButton;
}

void UPlayerActionWidget::DelayToSwitchToCharacterMoveState()
{
	GetWorld()->GetTimerManager().SetTimer(UIDelayTimerHandle, this, &UPlayerActionWidget::SwitchToCharacterMoveState, UIDelayTime, false);
}

void UPlayerActionWidget::SetupPlayerVariable()
{
	PlayerOwner = GetOwningPlayer<ABoardController>();
	LockedPlayerState = PlayerOwner->GetPlayerState<ALockedPlayerState>();
}

void UPlayerActionWidget::EndTurn()
{
	ABoardController* BoardController = GetOwningPlayer<ABoardController>();

	LastFocusedButton = nullptr;

	if (PlayerOwner)
	{
		PlayerOwner->DrawCardFromDeck();
		//PlayerOwner->Client_ShowIndicatorLayout(false);
	}
}

void UPlayerActionWidget::DelayToEndTurn()
{
	GetWorld()->GetTimerManager().SetTimer(UIDelayTimerHandle, this, &UPlayerActionWidget::EndTurn, UIDelayTime, false);
}

void UPlayerActionWidget::SwitchToTileMoveState()
{
	ABoardController* BoardController = GetOwningPlayer<ABoardController>();

	if (PlayerOwner && BoardController)
	{
		PlayerOwner->Server_ChangeCameraBehaviour(EMovementInputState::E_TileMovement);
		PlayerOwner->Client_ChangeIndicatorLayout(EActionLayout::ViewMapAction);

		BoardController->SetInputMode(FInputModeGameOnly());
	}

	SetVisibility(ESlateVisibility::Hidden);
	LastFocusedButton = ViewMapButton;
}

void UPlayerActionWidget::SetItemAsLastFocusedButton()
{
	//The other response implementation is in blueprint
	LastFocusedButton = ItemButton;
}

void UPlayerActionWidget::VisibilityChange(ESlateVisibility InVisibility)
{
	if (InVisibility == ESlateVisibility::SelfHitTestInvisible)
	{
		PlayerOwner->Client_ChangeIndicatorLayout(EActionLayout::MenuAction);
		PlayerOwner->Client_ShowIndicatorLayout(true);

		SetFocus();
	}
}

void UPlayerActionWidget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerActionWidget, PlayerOwner);
	DOREPLIFETIME(UPlayerActionWidget, LockedPlayerState);
}