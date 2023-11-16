// Fill out your copyright notice in the Description page of Project Settings.


#include "LeaveLobbyDoor.h"

void ALeaveLobbyDoor::Interact(APawn* InteractionInstigator)
{
	LeaveLobbyFromDoor(OverlappingCharacter);
}
