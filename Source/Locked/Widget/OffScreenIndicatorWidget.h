// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OffScreenIndicatorWidget.generated.h"

/**
 *
 */
UCLASS()
class LOCKED_API UOffScreenIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
		TArray<class AOffScreenIndicator*> PlayerPosInfo;

	void AddPlayerInfo(AOffScreenIndicator* NewInfo);
	
	UFUNCTION(BlueprintImplementableEvent)
		void DrawIndicator();
};
