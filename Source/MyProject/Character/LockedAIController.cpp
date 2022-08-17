// Fill out your copyright notice in the Description page of Project Settings.


#include "LockedAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "MyProject/Room/RoomTile.h"

ALockedAIController::ALockedAIController()
{
}

void ALockedAIController::AIMoveToRoom(ARoomTile* RoomToMove)
{
	GetBlackboardComponent()->SetValueAsObject("TargetRoom", RoomToMove);
}
