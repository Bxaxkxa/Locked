// Fill out your copyright notice in the Description page of Project Settings.


#include "LockedPlayerState.h"
#include "Net/UnrealNetwork.h"

ALockedPlayerState::ALockedPlayerState()
{
	SetReplicates(true);
}

void ALockedPlayerState::Server_ChangePlayerCameraBehaviour_Implementation()
{
	OnMovementBehaviourChangeDelegate.Execute(CurrentMovementInputState);
}

void ALockedPlayerState::Server_RefreshMovePoint_Implementation()
{
	AvailableMove = 3;
}

void ALockedPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALockedPlayerState, AvailableMove);
	DOREPLIFETIME(ALockedPlayerState, OnMovementBehaviourChangeDelegate);
}

