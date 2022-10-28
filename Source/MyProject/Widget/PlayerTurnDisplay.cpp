// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerTurnDisplay.h"
#include "MyProject/GameMode/MyProjectGameMode.h"
#include "MyProject/Character/BoardController.h"

void UPlayerTurnDisplay::NativeConstruct()
{
	FWidgetAnimationDynamicEvent DisplayDelegate;
	DisplayDelegate.BindDynamic(this, &UPlayerTurnDisplay::Server_StartPlayerTurn);

	BindToAnimationFinished(ShowTurn, DisplayDelegate);
}

void UPlayerTurnDisplay::Server_StartPlayerTurn_Implementation()
{
	/*AMyProjectGameMode* GameMode = (AMyProjectGameMode*)GetWorld()->GetAuthGameMode();
	if (GameMode)
	{
		GameMode->NextPlayerTurn();
	}*/

	ABoardController* BoardController = GetOwningPlayer<ABoardController>();

	if (BoardController && BoardController->IsItPlayersTurn())
	{
		BoardController->ShowActionWidget(true);
	}
}

void UPlayerTurnDisplay::PlayShowTurnAnimation(int PlayerTurn)
{
	CurrentPlayerTurn = PlayerTurn + 1;

	PlayAnimation(ShowTurn);
}
