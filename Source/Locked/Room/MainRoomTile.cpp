// Fill out your copyright notice in the Description page of Project Settings.


#include "MainRoomTile.h"
#include "Components/ChildActorComponent.h"
#include "Locked/GameState/LockedGameState.h"
#include "Net/UnrealNetwork.h"

AMainRoomTile::AMainRoomTile()
{
	LockedDoorActor = CreateDefaultSubobject<UChildActorComponent>("LockedDoorMesh");
	LockedDoorActor->SetupAttachment(RootComponent);
}

void AMainRoomTile::BeginPlay()
{
	Super::BeginPlay();

	Multicast_UpdateWidgetCount(NeededKey);
}

void AMainRoomTile::SpawnDoor()
{
}

bool AMainRoomTile::RoomEvent(APlayerControlPawn* Player)
{
	return false;
}

void AMainRoomTile::AddKey()
{
	NeededKey--;

	Multicast_UpdateWidgetCount(NeededKey);

	if (!NeededKey)
	{
		//TO DO: Win
		GetWorld()->ServerTravel("WinLevel");
	}
}

void AMainRoomTile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainRoomTile, NeededKey);
}
