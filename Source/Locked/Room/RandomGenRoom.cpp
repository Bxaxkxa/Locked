// Fill out your copyright notice in the Description page of Project Settings.


#include "RandomGenRoom.h"
#include "Locked/Room/RoomTile.h"
#include "Locked/SaveFile/BoardSettingsSave.h"
#include "Locked/Struct/GameSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ARandomGenRoom::ARandomGenRoom()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
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

	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Emerald, FString::FromInt(TotalTileCount));
}

int ARandomGenRoom::GetAllRemainingTileCount()
{
	int Num = 0;
	for (int i = 0; i < static_cast<int>(ERoomTileType::Num); i++)
	{
		Num += TileCards[i].GetRemainingTileAmounts();
	}
	return Num;
}

void ARandomGenRoom::GenerateTile_Implementation(UBoardSettingsSave* GameSetting)
{
	TArray<int> SettingTileCount = GameSetting->GetGameSettingsSave().GetTilesCount();
	TArray<float> SettingTileChance = GameSetting->GetGameSettingsSave().GetTilesChance();

	bIsUsingChance = GameSetting->GetGameSettingsSave().IsUsingChance();

	for (int i = 0; i < static_cast<int>(ERoomTileType::Num); i++)
	{
		TileCards[i].SetTileAmounts(SettingTileCount[i]);
		TileCards[i].SetTileChance(SettingTileChance[i]);

		TotalTileCount += SettingTileCount[i];
	}
}

ARoomTile* ARandomGenRoom::DrawTile()
{
	return bIsUsingChance ? DrawInfiniteTile() : DrawFiniteTile();
}

ARoomTile* ARandomGenRoom::DrawInfiniteTile()
{
	int32 RandNumber;

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return nullptr;
	}
	bool NotValidRoom = true;
	RandNumber = FMath::RandRange(0, 100);

	TSubclassOf<ARoomTile> UsedTemplate;
	int count = 0;
	for (FRoomTileCard it : TileCards)
	{
		if (!it.GetTileChances())
		{
			continue;
		}

		count += it.GetTileChances();
		if (RandNumber <= count)
		{
			UsedTemplate = it.GetRoomTemplate();
		}
	}

	if (UsedTemplate)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = Cast<APawn>(GetOwner());
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FTransform spawnlocation = GetActorTransform();
		ARoomTile* SpawnTile = World->SpawnActor<ARoomTile>(UsedTemplate, spawnlocation, SpawnParams);

		//TileCards[RandNumber].ReduceTilesAmount();

		//RNG to spawn treasure room
		RandNumber = FMath::RandRange(0, 5);
		//SpawnTile->bIsTreasureRoom = true;
		//SpawnTile->bIsTreasureRoom = false;
		SpawnTile->bIsTreasureRoom = !RandNumber;

		return SpawnTile;
	}

	return nullptr;
}

ARoomTile* ARandomGenRoom::DrawFiniteTile()
{
	int32 RandNumber;

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return nullptr;
	}

	//If every tile type except for the single room tile type is empty
	if (!TileCards[QuadDoorTile].GetRemainingTileAmounts() &&
		!TileCards[TripleDoorTile].GetRemainingTileAmounts() &&
		!TileCards[DoubleStraightDoorTile].GetRemainingTileAmounts() &&
		!TileCards[DoubleCurveDoorTile].GetRemainingTileAmounts() &&
		!TileCards[SingleDoorTile].GetRemainingTileAmounts())
	{
		return nullptr;
	}

	TArray<AActor*> OutResult;
	UGameplayStatics::GetAllActorsOfClass(World, ARoomTile::StaticClass(), OutResult);
	bool NotValidRoom = true;
	//Do loop 
	do
	{
		RandNumber = FMath::RandRange(0, Num - 1);

		//If there is no more tile in the deck with the tile type that is going to be drawn
		if (!TileCards[RandNumber].GetRemainingTileAmounts())
		{
			continue;
		}

		//If every tile type except for the single room tile type is empty
		if (!TileCards[QuadDoorTile].GetRemainingTileAmounts() &&
			!TileCards[TripleDoorTile].GetRemainingTileAmounts() &&
			!TileCards[DoubleStraightDoorTile].GetRemainingTileAmounts() &&
			!TileCards[DoubleCurveDoorTile].GetRemainingTileAmounts())
		{
			break;
		}

		//If going to generate single door room
		if (RandNumber == SingleDoorTile)
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

		//TileCards[RandNumber].ReduceTilesAmount();
		TileCards[RandNumber].TileAmounts -= 1;
		TotalTileCount--;

		//RNG to spawn treasure room
		RandNumber = FMath::RandRange(0, 5);
		//SpawnTile->bIsTreasureRoom = true;
		//SpawnTile->bIsTreasureRoom = false;
		SpawnTile->bIsTreasureRoom = !RandNumber;

		return SpawnTile;
	}

	return nullptr;
}

void ARandomGenRoom::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARandomGenRoom, TotalTileCount);
}
