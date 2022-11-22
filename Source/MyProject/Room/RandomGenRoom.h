// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RandomGenRoom.generated.h"

class ARoomTile;

USTRUCT(BlueprintType)
struct FRoomTileCard
{
	GENERATED_BODY();
private:
	UPROPERTY(EditAnywhere)
		TSubclassOf<ARoomTile> RoomTemplate;
	UPROPERTY(EditAnywhere)
		int TileAmounts;

public:
	void SetTileAmounts(int Num) { TileAmounts = Num; }
	void ReduceTilesAmount() { TileAmounts -= 1; }

	TSubclassOf<ARoomTile> GetRoomTemplate() { return RoomTemplate; }

	int GetRemainingTileAmounts() { return TileAmounts; }
};

UENUM()
enum ERoomTileType
{
	E_QuadDoorTile = 0,
	E_TripleDoorTile = 1,
	E_DoubleStraightDoorTile = 2,
	E_DoubleCurveDoorTile = 3,
	E_SingleDoorTile = 4,
	E_Num = 5
};

UCLASS()
class MYPROJECT_API ARandomGenRoom : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARandomGenRoom();

	uint8 NumberOfATile = 10;
	uint8 NumberOfBTile = 10;
	uint8 NumberOfCTile = 10;
	uint8 NumberOfDTile = 10;

	//UPROPERTY(EditAnywhere)
	//	TSubclassOf<ARoomTile> RoomTemplates[4];

	UPROPERTY(EditAnywhere)
		FRoomTileCard TileCards[ERoomTileType::E_Num];

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	ARoomTile* DrawTile();

};
