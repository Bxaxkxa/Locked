// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomTile.h"

// Sets default values
ARoomTile::ARoomTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	NeighbourRoom.Add("NextLeft");
	NeighbourRoom.Add("NextRight");
	NeighbourRoom.Add("NextUp");
	NeighbourRoom.Add("NextDown");
}

// Called when the game starts or when spawned
void ARoomTile::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ARoomTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

