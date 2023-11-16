// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM()
enum class EMovementInputState
{
	E_Idle,
	E_CharMovement,
	E_MoveTileMovement, //TileMovement when change from CharMovement
	E_TileMovement, //TileMovement when change from action menu
	E_TilePlacement,
	E_RifleTarget,
	E_HammerTarget
};

