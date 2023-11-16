// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HeartIcon.generated.h"

/**
 * 
 */
UCLASS()
class LOCKED_API UHeartIcon : public UUserWidget
{
	GENERATED_BODY()
	
private:
	bool bIsActive = true;

public:
	UPROPERTY(Transient, meta = (BindWidgetAnim), BlueprintReadOnly)
		UWidgetAnimation* ActivateHealthAnim;

	UPROPERTY(Transient, meta = (BindWidgetAnim), BlueprintReadOnly)
		UWidgetAnimation* DeactivateHealthAnim;

	UFUNCTION(BlueprintCallable)
		void ActivateHealth(bool isBeingActived);
};
