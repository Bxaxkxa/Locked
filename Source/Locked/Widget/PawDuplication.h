// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PawDuplication.generated.h"

/**
 *
 */
UCLASS()
class LOCKED_API UPawDuplication : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		UUserWidget* SourceWidget;

	UPROPERTY(BlueprintReadOnly)
		bool bIsDuel = false;

	void SetIsForDuel(bool IsForDuel) { bIsDuel = IsForDuel; }
};
