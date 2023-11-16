// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EActionLayout : uint8
{
	MenuAction = 0,
	MenuWithBackAction = 1,
	MoveAction = 2,
	ItemAction = 3,
	ViewMapAction = 4,
	PlaceTileAction = 5,
	DiceRoll = 6,
	TrapItemAction = 7,
	OverCapacityAction = 8,
	NUM = 9,
};

UENUM(BlueprintType)
enum class EInventoryType : uint8
{
	PlayerInventory = 0,
	OverCapacity = 1,
	StealItem = 2,
	NUM = 3,
};
