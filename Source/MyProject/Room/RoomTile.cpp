// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomTile.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ARoomTile::ARoomTile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RoomDetectionCollider = CreateDefaultSubobject<UBoxComponent>("RoomDetectionCollider");
	RoomDetectionCollider->SetCollisionProfileName("OverlapOnlyPawn");
	SetRootComponent(RoomDetectionCollider);

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ARoomTile::BeginPlay()
{
	Super::BeginPlay();

	//Top Right
	PlayerIdlePositions[0] = FVector(PlayerIdleDistance, PlayerIdleDistance,0);
	//Top Left
	PlayerIdlePositions[1] = FVector(-PlayerIdleDistance, PlayerIdleDistance, 0);
	//Bottom Left
	PlayerIdlePositions[2] = FVector(-PlayerIdleDistance,-PlayerIdleDistance, 0);
	//Bottom Right
	PlayerIdlePositions[3] = FVector(PlayerIdleDistance, -PlayerIdleDistance, 0);
}

// Called every frame
void ARoomTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARoomTile::CheckNeightbourRooms()
{
	for (int i = 0; i < static_cast<int>(ETileDirection::NUM); i++)
	{
		if (NeighbourRoom[i].GetNextRoom() || !NeighbourRoom[i].IsThereDoorway())
		{
			continue;
		}

		bool IsGoingLeft = ETileDirection(i) == ETileDirection::NextLeft;
		bool IsGoingRight = ETileDirection(i) == ETileDirection::NextRight;
		bool IsGoingUp = ETileDirection(i) == ETileDirection::NextUp;
		bool IsGoingDown = ETileDirection(i) == ETileDirection::NextDown;
		FVector Direction = FVector((IsGoingUp * 1.0f) + (IsGoingDown * -1.0f), (IsGoingRight * 1.0f) + (IsGoingLeft * -1.0f), 0);
		FVector AddPosition = Direction * 200.0f;
		FVector TraceStart = GetActorLocation() + AddPosition;

		FVector TraceEnd = TraceStart + Direction * 200.0f;

		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);
		TraceParams.bTraceComplex = true;
		TraceParams.bReturnPhysicalMaterial = false;

		UWorld* World = GetWorld();
		FHitResult OutHit;

		if (!World->LineTraceSingleByChannel(OutHit, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, TraceParams))
		{
			continue;
		}
		ARoomTile* HitRoom = Cast<ARoomTile>(OutHit.GetActor());

		ETileDirection OppositeDirection;
		int DirectionEnumBit = i;
		if (DirectionEnumBit >= 2)
		{
			OppositeDirection = ETileDirection(DirectionEnumBit - 2);
		}
		else
		{
			OppositeDirection = ETileDirection(DirectionEnumBit + 2);
		}

		if (!HitRoom->NeighbourRoom[static_cast<int>(OppositeDirection)].IsThereDoorway())
		{
			continue;
		}

		NeighbourRoom[i].SetNextRoom(HitRoom);
		HitRoom->SetOppositeRoom(ETileDirection(i), this);
	}
}

void ARoomTile::SetOppositeRoom(ETileDirection OriginDirection, ARoomTile* OriginRoom)
{
	ETileDirection OppositeDirection;
	int DirectionEnumBit = (int)OriginDirection;
	if (DirectionEnumBit >= 2)
	{
		OppositeDirection = ETileDirection(DirectionEnumBit - 2);
	}
	else
	{
		OppositeDirection = ETileDirection(DirectionEnumBit + 2);
	}

	NeighbourRoom[(int)OppositeDirection].SetNextRoom(OriginRoom);
}

void ARoomTile::RotateRoomPlacement()
{
	FDoorWay LastDoorWayReplacement = NeighbourRoom[(int)ETileDirection::NextRight];
	
	for (int i = 0; i < (int)ETileDirection::NextDown; i++)
	{
		NeighbourRoom[i] = NeighbourRoom[i + 1];
	}

	NeighbourRoom[(int)ETileDirection::NextDown] = LastDoorWayReplacement;

	FRotator NewRotation = GetActorRotation() + FRotator(0.0f, 90.0f, 0.0f);
	SetActorRotation(NewRotation);
}

void ARoomTile::CheckConnectionAvailibility(ETileDirection OriginDirection)
{
	ETileDirection OppositeDirection;
	int DirectionEnumBit = (int)OriginDirection;
	if (DirectionEnumBit >= 2)
	{
		OppositeDirection = ETileDirection(DirectionEnumBit - 2);
	}
	else
	{
		OppositeDirection = ETileDirection(DirectionEnumBit + 2);
	}

	while (!NeighbourRoom[(int)OppositeDirection].IsThereDoorway())
	{
		RotateRoomPlacement();
	}
}

FVector ARoomTile::GetNextAvailableIdleSpot()
{
	FVector PlayerIdlePositionTarget = GetActorLocation() + PlayerIdlePositions[0];
	return PlayerIdlePositionTarget;
}

void ARoomTile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARoomTile, NeighbourRoom);
}
