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
	Sword = 2,
	Rifle = 3,
	Hammer = 4,
	SpikeArmor = 5,
	EnergyDrink = 6,
	UtilityKit = 7,
	SmokeBomb = 8,
	DarkEffigy = 9,
	Tome = 10,
	MonkeysPaw = 11,
	SpikeTrap = 12,
	ArrowTrap = 13,
	NetTrap = 14,
	BearTrap = 15,
	Key = 16,
	Num = 17,
};
ENUM_RANGE_BY_COUNT(EItemList, EItemList::Num);

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon,
	Consumable,
	Trap,
	ConsumableWeapon,
	Key,
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bIsTreasure = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int ItemIdNumber;

	FItemData() {}
	FItemData(const FItemData& Data, int NewIdNumber) : ItemName(Data.ItemName), ItemDescription(Data.ItemDescription),
		ItemImage(Data.ItemImage), ItemType(Data.ItemType), bIsTreasure(Data.bIsTreasure), ItemIdNumber(NewIdNumber) {}

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
		int CardAmounts;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FItemData ItemData;

	void SetCardAmounts(int Num) { CardAmounts = Num; }

	FItemData GetItemData() { return ItemData; }

	int GetCardAmounts() { return CardAmounts; }
};
