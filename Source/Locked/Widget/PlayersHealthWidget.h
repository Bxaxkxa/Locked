// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Locked/Struct/CardStruct.h"
#include "PlayersHealthWidget.generated.h"

/**
 *
 */
UCLASS()
class LOCKED_API UPlayersHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
		TSubclassOf <class UHeartContainer> ContainerBPClass;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
		class UHorizontalBox* ContainerBox;

	UPROPERTY(BlueprintReadWrite)
		TArray<UHeartContainer*> PlayersHealthContainers;

	void SetupPlayerInfo(TArray<FString> PlayerNames);

	void UpdatePlayerHealth(TArray<int> PlayersHealth);
	void UpdatePlayerInventory(TArray<int> PlayersItemCount, TArray<int> PlayersTreasureCount);
};
