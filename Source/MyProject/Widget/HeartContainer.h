// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HeartContainer.generated.h"

/**
 *
 */
UCLASS()
class MYPROJECT_API UHeartContainer : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		TArray<class UHeartIcon*> Hearts;

	void UpdatePlayerHealth(int UpdatedHealthPoint);
};
