// Fill out your copyright notice in the Description page of Project Settings.


#include "ReadyDoor.h"
#include "Locked/GameMode/LobbyGameMode.h"
#include "Locked/Character/LobbyCharacter.h"
#include "Net/UnrealNetwork.h"

void AReadyDoor::Interact(APawn* InteractionInstigator)
{
	ALobbyCharacter* LobbyChar = Cast<ALobbyCharacter>(OverlappingCharacter);

	if (LobbyChar)
	{
		LobbyChar->ReadyUp();
		OverlappingCharacter = nullptr;
	}
}
