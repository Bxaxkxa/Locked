// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SelectedButton.generated.h"

/**
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickDelegate);

UCLASS()
class MYPROJECT_API USelectedButton : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, BLueprintCallable)
		FOnClickDelegate OnButtonClick;
};
