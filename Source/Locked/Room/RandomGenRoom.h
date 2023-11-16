// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Locked/Enum/RoomTileEnum.h"
#include "RandomGenRoom.generated.h"

class ARoomTile;

USTRUCT(BlueprintType)
struct FRoomTileCard
{
	GENERATED_BODY();
public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<ARoomTile> RoomTemplate;
	UPROPERTY(EditAnywhere)
		int TileAmounts;
	UPROPERTY(EditAnywhere)
		float TileChances;

public:
	void SetTileAmounts(int Num) { TileAmounts = Num; }
	void SetTileChance(float Num) { TileChances = Num; }
	void ReduceTilesAmount() { TileAmounts -= 1; }

	TSubclassOf<ARoomTile> GetRoomTemplate() { return RoomTemplate; }

	int GetRemainingTileAmounts() { return TileAmounts; }
	float GetTileChances() { return TileChances; }
};

UCLASS()
class LOCKED_API ARandomGenRoom : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARandomGenRoom();

	//UPROPERTY(EditAnywhere)
	//	TSubclassOf<ARoomTile> RoomTemplates[4];

	UPROPERTY(EditAnywhere)
	//UPROPERTY(EditAnywhere)
		FRoomTileCard TileCards[ERoomTileType::Num];

	UPROPERTY(BlueprintReadOnly)
		bool bIsUsingChance;

	UPROPERTY(BlueprintReadOnly, Replicated)
		int TotalTileCount;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		int GetAllRemainingTileCount();

	UFUNCTION(Server,Reliable)
	void GenerateTile(class UBoardSettingsSave* GameSetting);

	ARoomTile* DrawTile();
	ARoomTile* DrawFiniteTile();
	ARoomTile* DrawInfiniteTile();

};
