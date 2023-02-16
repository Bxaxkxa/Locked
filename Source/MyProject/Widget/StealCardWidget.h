// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyProject/Struct/CardStruct.h"
#include "StealCardWidget.generated.h"

/**
 *
 */
UCLASS()
class MYPROJECT_API UStealCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
		TArray<FItemData> StoledInventory;

	void ShuffleItemCardPosition(TArray<FItemData> ItemCards);

	UFUNCTION(BlueprintCallable)
		void StealItem(FItemData StolenItem);
};
