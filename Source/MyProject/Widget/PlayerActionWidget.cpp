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
	ViewMapButton->OnButtonClick.AddDynamic(this, &UPlayerActionWidget::SwitchToTileMoveState);
	EndTurnButton->OnButtonClick.AddDynamic(this, &UPlayerActionWidget::DelayToEndTurn);

	OnVisibilityChanged.AddDynamic(this, &UPlayerActionWidget::VisibilityChange);
}

void UPlayerActionWidget::SwitchToCharacterMoveState()
{
	ABoardController* BoardController = GetOwningPlayer<ABoardController>();

	if (PlayerOwner)
	{
		PlayerOwner->Server_ChangeCameraBehaviour(EMovementInputState::E_CharMovement);
		PlayerOwner->Client_ChangeIndicatorLayout(EActionLayout::MoveAction);
	}

	SetVisibility(ESlateVisibility::Hidden);
	//RemoveFromParent();
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

	if (PlayerOwner)
	{
		PlayerOwner->Multicast_EndTurn();
		PlayerOwner->Client_ShowIndicatorLayout(false);
	}
}

void UPlayerActionWidget::DelayToEndTurn()
{
	GetWorld()->GetTimerManager().SetTimer(UIDelayTimerHandle, this, &UPlayerActionWidget::EndTurn, UIDelayTime, false);
}

void UPlayerActionWidget::SwitchToTileMoveState()
{
	ABoardController* BoardController = GetOwningPlayer<ABoardController>();

	if (PlayerOwner)
	{
		PlayerOwner->Server_ChangeCameraBehaviour(EMovementInputState::E_TileMovement);
		PlayerOwner->Client_ChangeIndicatorLayout(EActionLayout::ViewMapAction);
	}

	SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerActionWidget::VisibilityChange(ESlateVisibility InVisibility)
{
	if (InVisibility == ESlateVisibility::SelfHitTestInvisible)
	{
		MoveActionButton->SetFocus();

		PlayerOwner->Client_ChangeIndicatorLayout(EActionLayout::MenuAction);
		PlayerOwner->Client_ShowIndicatorLayout(true);
	}
}

void UPlayerActionWidget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerActionWidget, PlayerOwner);
	DOREPLIFETIME(UPlayerActionWidget, LockedPlayerState);
}