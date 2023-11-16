// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyLoadingScreen.generated.h"

/**
 * 
 */
UCLASS()
class LOCKED_API ULobbyLoadingScreen : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Transient, meta = (BindWidgetAnim), BlueprintReadOnly)
		UWidgetAnimation* LoadingFadeOffAnimation;

	//void StartFadeOffAnimation();

	//UFUNCTION(BlueprintCallable)
	//	void StartLoadingTimer();

	//UPROPERTY(BlueprintAssignable, BlueprintCallable)
	//	FOnFinishLoading OnFinishLoading;
};
