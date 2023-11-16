// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomTileWallMesh.h"

void URoomTileWallMesh::Break(bool bIsBreak)
{
	SetVisibility(!bIsBreak);
	bIsBreak ? SetCollisionEnabled(ECollisionEnabled::NoCollision) : SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

bool URoomTileWallMesh::IsDoorOfDirection(ETileDirection CheckDirection)
{
	return WallDirection == CheckDirection;
}
