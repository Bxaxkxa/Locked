// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum ERoomTileType
{
	QuadDoorTile = 0,
	TripleDoorTile = 1,
	DoubleStraightDoorTile = 2,
	DoubleCurveDoorTile = 3,
	SingleDoorTile = 4,
	Num = 5
};
