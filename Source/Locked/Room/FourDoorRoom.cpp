// Fill out your copyright notice in the Description page of Project Settings.


#include "FourDoorRoom.h"

AFourDoorRoom::AFourDoorRoom()
{
	SetReplicates(true);
	SetReplicateMovement(true);
	
}

void AFourDoorRoom::SpawnDoor()
{
	UWorld* World = GetWorld();

	//World->SpawnActor()
}
