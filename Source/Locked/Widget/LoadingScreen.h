// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingScreen.generated.h"

/**
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishLoading);

UCLASS()
class LOCKED_API ULoadingScreen : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient, meta = (BindWidgetAnim), BlueprintReadOnly)
		UWidgetAnimation* LoadingFadeOffAnimation;

	UPROPERTY(Transient, meta = (BindWidgetAnim), BlueprintReadOnly)
		UWidgetAnimation* LoadingFadeInAnimation;

	void StartFadeOffAnimation();
	void StartFadeInAnimation();
};
