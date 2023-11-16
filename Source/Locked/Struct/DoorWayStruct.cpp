// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorWayStruct.h"
#include "Components/StaticMeshComponent.h"

void FDoorWay::BreakWall(bool bState)
{
	bHaveDoorWay = bState; 
}
