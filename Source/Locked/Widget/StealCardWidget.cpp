// Fill out your copyright notice in the Description page of Project Settings.


#include "StealCardWidget.h"
#include "Locked/Character/BoardController.h"
#include "Locked/BlueprintLibrary/LockedBFL.h"

void UStealCardWidget::ShuffleItemCardPosition(TArray<FItemData> ItemCards)
{
	StoledInventory = ULockedBFL::ShuffleArray(ItemCards);
}

void UStealCardWidget::StealItem(FItemData StolenItem)
{
	GetOwningPlayer<ABoardController>()->Server_StealOpponentItem(StolenItem);
}
