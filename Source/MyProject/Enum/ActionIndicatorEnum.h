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
	NUM = 5,
};
