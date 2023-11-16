// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerActionWidget.h"
#include "Locked/Character/BoardController.h"
#include "Locked/Character/State/LockedPlayerState.h"
#include "Locked/Widget/ActionIndicatorLayout.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void UPlayerActionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GetWorld()->GetTimerManager().SetTimer(UIDelayTimerHandle, this, &UPlayerActionWidget::SetupPlayerVariable, UIDelayTime, false);

	if (MoveActionButton->OnKeyboardClick.IsBound())
		return;

	MoveActionButton->OnKeyboardClick.AddDynamic(this, &UPlayerActionWidget::DelayToSwitchToCharacterMoveState);
	ItemButton->OnKeyboardClick.AddDynamic(this, &UPlayerActionWidget::SetItemAsLastFocusedButton);
	ViewMapButton->OnKeyboardClick.AddDynamic(this, &UPlayerActionWidget::SwitchToTileMoveState);
	EndTurnButton->OnKeyboardClick.AddDynamic(this, &UPlayerActionWidget::DelayToEndTurn);

	OnVisibilityChanged.AddDynamic(this, &UPlayerActionWidget::VisibilityChange);
}

void UPlayerActionWidget::SwitchToCharacterMoveState()
{
	if (PlayerOwner)
	{
		PlayerOwner->SetPlayerMovementStateToMoveState();
	}
	
	//UWidgetBlueprintLibrary::SetFocusToGameViewport();
	//UnFocus();
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

		BoardController->Client_SetControllerInputMode(true);
	}

	//UWidgetBlueprintLibrary::SetFocusToGameViewport();
	//UnFocus();
	SetVisibility(ESlateVisibility::Hidden);
	LastFocusedButton = ViewMapButton;
}

void UPlayerActionWidget::SetItemAsLastFocusedButton()
{
	//The other response implementation is in blueprint
	LastFocusedButton = ItemButton;
}

void UPlayerActionWidget::UnFocus_Implementation()
{
	//UWidgetBlueprintLibrary::SetFocusToGameViewport();
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