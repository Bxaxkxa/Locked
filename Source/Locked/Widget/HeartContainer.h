// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HeartContainer.generated.h"

/**
 *
 */
UCLASS()
class LOCKED_API UHeartContainer : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		TArray<class UHeartIcon*> Hearts;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
		class UTextBlock* PlayerSteamID;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* ItemCount;

	UPROPERTY(meta = (BindWidget))
		class UTextBlock* TreasureCount;

	void SetSteamID(FString SteamID);
	void UpdatePlayerHealth(int UpdatedHealthPoint);
	void UpdateItemCount(int UpdatedItemCount);
	void UpdateTreasureCount(int UpdatedTreasureCount);
};
