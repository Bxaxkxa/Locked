// Fill out your copyright notice in the Description page of Project Settings.


#include "RandomGenRoom.h"
#include "MyProject/Room/RoomTile.h"
#include "Kismet/GameplayStatics.h"

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
	int32 RandNumber;

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return nullptr;
	}
	TArray<AActor*> OutResult;
	UGameplayStatics::GetAllActorsOfClass(World, ARoomTile::StaticClass(), OutResult);
	bool NotValidRoom = true;
	//Do loop 
	do
	{
		RandNumber = FMath::RandRange(0, E_Num - 1);

		if (!TileCards[RandNumber].GetRemainingTileAmounts())
		{
			continue;
		}

		if (!TileCards[E_QuadDoorTile].GetRemainingTileAmounts() &&
			!TileCards[E_TripleDoorTile].GetRemainingTileAmounts() &&
			!TileCards[E_DoubleStraightDoorTile].GetRemainingTileAmounts() &&
			!TileCards[E_DoubleCurveDoorTile].GetRemainingTileAmounts())
		{
			break;
		}

		//If going to generate single door room
		if (RandNumber == E_SingleDoorTile)
		{
			uint8 NotFullyConnectedRoom = 0;
			//Loop every generated room
			for (AActor* it : OutResult)
			{
				//If there is not fully connected room
				if (!Cast<ARoomTile>(it)->FullyConnected)
				{
					NotFullyConnectedRoom++;
					//If there is more than 2 not fully connected room
					if (NotFullyConnectedRoom >= 2)
					{
						//Set state to stop loop
						NotValidRoom = false;
						break;
					}
				}
			}
			continue;
		}
		//If it is not single door tile stop loop
		NotValidRoom = false;

	} while (NotValidRoom);

	if (TileCards[RandNumber].GetRoomTemplate())
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = Cast<APawn>(GetOwner());
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FTransform spawnlocation = GetActorTransform();
		ARoomTile* SpawnTile = World->SpawnActor<ARoomTile>(TileCards[RandNumber].GetRoomTemplate(), spawnlocation, SpawnParams);

		TileCards[RandNumber].ReduceTilesAmount();

		return SpawnTile;
	}

	return nullptr;
}

