// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerActionWidget.h"
#include "MyProject/Character/BoardController.h"
#include "MyProject/Character/State/LockedPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void UPlayerActionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GetWorld()->GetTimerManager().SetTimer(UIDelayTimerHandle, this, &UPlayerActionWidget::SetupPlayerVariable, UIDelayTime, false);

	if (MoveActionButton->OnClicked.IsBound())
		return;

	MoveActionButton->OnClicked.AddDynamic(this, &UPlayerActionWidget::DelayToSwitchToCharacterMoveState);
	ViewMapButton->OnClicked.AddDynamic(this, &UPlayerActionWidget::SwitchToTileMoveState);
	EndTurnButton->OnClicked.AddDynamic(this, &UPlayerActionWidget::DelayToEndTurn);
}

void UPlayerActionWidget::SwitchToCharacterMoveState()
{
	ABoardController* BoardController = GetOwningPlayer<ABoardController>();

	if (PlayerOwner)
	{
		PlayerOwner->Server_ChangeCameraBehaviour(EMovementInputState::E_CharMovement);
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
	}

	SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerActionWidget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPlayerActionWidget, PlayerOwner);
	DOREPLIFETIME(UPlayerActionWidget, LockedPlayerState);
}