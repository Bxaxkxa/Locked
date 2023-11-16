// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DoorWayStruct.generated.h"

USTRUCT(BlueprintType)
struct FDoorWay
{
	GENERATED_BODY();
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		class ARoomTile* NextRoom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bHaveDoorWay;
public:
	void BreakWall(bool bState);

	void SetNextRoom(ARoomTile* Room) { NextRoom = Room; }

	ARoomTile* GetNextRoom() { return NextRoom; }

	bool IsThereDoorway() {	return bHaveDoorWay; }
};