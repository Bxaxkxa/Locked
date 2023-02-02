// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CardStruct.generated.h"

UENUM(BlueprintType)
enum class EItemList : uint8
{
	Fist = 0,
	Dagger = 1,
	EnergyDrink = 2,
	SpikeTraps = 3,
	Num = 4,
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon,
	Consumable,
	Trap,
};

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_USTRUCT_BODY();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EItemList ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSlateBrush ItemImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EItemType ItemType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int ItemIdNumber;

	FItemData() {}
	FItemData(const FItemData& Data, int NewIdNumber) : ItemName(Data.ItemName), ItemDescription(Data.ItemDescription),
		ItemImage(Data.ItemImage), ItemType(Data.ItemType), ItemIdNumber(NewIdNumber) {}

	FORCEINLINE void SetItemName(EItemList Name) { ItemName = Name; }

	bool operator==(const FItemData& other) const
	{
		return (other.ItemName == ItemName && other.ItemIdNumber == ItemIdNumber);
	}
};

USTRUCT(BlueprintType)
struct FItemCard
{
	GENERATED_BODY();
private:
	UPROPERTY(EditAnywhere)
		FItemData ItemData;
	UPROPERTY(EditAnywhere)
		int CardAmounts;

public:
	void SetCardAmounts(int Num) { CardAmounts = Num; }

	FItemData GetItemData() { return ItemData; }

	int GetCardAmounts() { return CardAmounts; }
};
