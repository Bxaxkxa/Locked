// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LobbyDoor.h"
#include "Locked/Interface/Interactable.h"
#include "ReadyDoor.generated.h"

/**
 * 
 */
UCLASS()
class LOCKED_API AReadyDoor : public ALobbyDoor
{
	GENERATED_BODY()

public:
	virtual void Interact(APawn* InteractionInstigator) override;

	//UFUNCTION(Server, Reliable)
		void ReadyUpPlayer();
};
