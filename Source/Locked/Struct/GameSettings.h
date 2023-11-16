// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Locked/Enum/RoomTileEnum.h"
#include "GameSettings.generated.h"

USTRUCT(BlueprintType)
struct FGameSettings
{
	GENERATED_BODY();
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		int PlayerNumber;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TArray<int> ItemCardCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TArray<int> TilesCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		TArray<float> TilesChance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bIsUsingChance = false;

public:
	void SetPlayerNumber(int Num) { PlayerNumber = Num; }
	void SetItemCardCount(TArray<int>  NewCardCount) { ItemCardCount = NewCardCount; }
	void SetTilesCount(TArray<int>  NewTilesCount) { TilesCount = NewTilesCount; }
	void SetTilesChance(TArray<float>  NewTilesChance) { TilesChance = NewTilesChance; }
	void SetUsingChance(bool WillUseChance) { bIsUsingChance = WillUseChance; }

	void ChangeItemCardCount(int NewCount, int CountIndex) { ItemCardCount[CountIndex] = NewCount; }
	void ChangeTileCount(int NewCount, int CountIndex) { TilesCount[CountIndex] = NewCount; }
	void ChangeTileChance(float NewChance, int ChanceIndex) { TilesChance[ChanceIndex] = NewChance; }
	void AddPlayerNumber() { PlayerNumber += 1; }

	int GetPlayerNumber() { return PlayerNumber; }
	TArray<int> GetItemCardCount() { return ItemCardCount; }
	TArray<int> GetTilesCount() { return TilesCount; }
	TArray<float> GetTilesChance() { return TilesChance; }
	bool IsUsingChance() { return bIsUsingChance; }
};