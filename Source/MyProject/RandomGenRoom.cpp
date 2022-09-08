// Fill out your copyright notice in the Description page of Project Settings.


#include "RandomGenRoom.h"
#include "MyProject/Room/RoomTile.h"

// Sets default values
ARandomGenRoom::ARandomGenRoom()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARandomGenRoom::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ARandomGenRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

ARoomTile* ARandomGenRoom::DrawTile()
{
	uint8 RandNumber = rand() % 4;

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return nullptr;
	}

	if (RoomTemplates[RandNumber])
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = Cast<APawn>(GetOwner());
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FTransform spawnlocation = GetActorTransform();
		return World->SpawnActor<ARoomTile>(RoomTemplates[RandNumber], spawnlocation, SpawnParams);
	}

	//switch (RandNumber)
	//{
	//case 0:
	//	GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, "Draw A Tile");
	//	break;
	//case 1:
	//	GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, "Draw B Tile");
	//	break;
	//case 2:
	//	GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, "Draw C Tile");
	//	break;
	//case 3:
	//	GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, "Draw D Tile");
	//	break;
	//default:
	//	break;
	//}

	return nullptr;
}

