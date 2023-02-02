// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CardStruct.h"
#include "DuelData.generated.h"

UENUM(BlueprintType)
enum class EDuelResult : uint8
{
	AttackerWin = 0,
	DefenderWin = 1,
	Draw = 2,
	Num = 3,
};

USTRUCT(BlueprintType)
struct FDuelData
{
	GENERATED_USTRUCT_BODY();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FItemData ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsReady = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool IsAttacker = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int WeaponValue = 0;

	FDuelData() {}
	FDuelData(bool IsDataForAttacker) : IsAttacker(IsDataForAttacker) {}
	FDuelData(FItemData NewItemData, bool IsDataForAttacker, int NewWeaponValue) : ItemData(NewItemData), IsReady(true), 
		IsAttacker(IsDataForAttacker), WeaponValue(NewWeaponValue) {}

	//FORCEINLINE void SetItemName(EItemList Name) { ItemName = Name; }

	bool operator==(const FDuelData& other) const
	{
		return (other.IsAttacker == IsAttacker);
	}
};

USTRUCT(BlueprintType)
struct FDuelDiceData
{
	GENERATED_USTRUCT_BODY();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsAttackerStillRolling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsDefenderStillRolling;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int32 AttackerDiceNumber;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		int DefenderDiceNumber;
};
