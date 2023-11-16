// Fill out your copyright notice in the Description page of Project Settings.


#include "MainRoomDoor.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AMainRoomDoor::AMainRoomDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>("DoorFrame");
	DoorFrame->SetCollisionProfileName("NoCollision");
	SetRootComponent(DoorFrame);

	LeftDoor = CreateDefaultSubobject<UStaticMeshComponent>("LeftDoor");
	LeftDoor->SetCollisionProfileName("NoCollision");
	LeftDoor->SetupAttachment(RootComponent);

	RightDoor = CreateDefaultSubobject<UStaticMeshComponent>("RightDoor");
	RightDoor->SetCollisionProfileName("NoCollision");
	RightDoor->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMainRoomDoor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMainRoomDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
