// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TextMessageWidget.generated.h"

/**
 *
 */
UCLASS()
class LOCKED_API UTextMessageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
		class UTextBlock* TextMessageBlock;

	UPROPERTY(Transient, meta = (BindWidgetAnim), BlueprintReadOnly)
		UWidgetAnimation* FadeOutAnimation;

	virtual void NativeConstruct() override;
	
	void PlayFadeOutAnimation();

	UFUNCTION()
		void VisibilityChange(ESlateVisibility InVisibility);
	UFUNCTION(BlueprintCallable)
		void SetTextMessage(FString NewMessage);
};
