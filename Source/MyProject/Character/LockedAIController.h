// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "LockedAIController.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ALockedAIController : public AAIController
{
	GENERATED_BODY()

public:
	ALockedAIController();

	void AIMoveToRoom(class ARoomTile* RoomToMove);
};
