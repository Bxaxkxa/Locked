// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LobbyDoor.h"
#include "Locked/Interface/Interactable.h"
#include "LeaveLobbyDoor.generated.h"

/**
 * 
 */
UCLASS()
class LOCKED_API ALeaveLobbyDoor : public ALobbyDoor
{
	GENERATED_BODY()

public:
	virtual void Interact(APawn* InteractionInstigator) override;

	UFUNCTION(BlueprintImplementableEvent)
		void LeaveLobbyFromDoor(APawn* LeavingPlayer);
};
