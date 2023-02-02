// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DuelOption.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UDuelOption : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
		class USelectedButton* ChallengeButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
		USelectedButton* IgnoreButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
		class UHorizontalBox* ChallengeRemainingBox;

	UPROPERTY(Transient, meta = (BindWidgetAnim) , BlueprintReadOnly)
		UWidgetAnimation* InitialFadeInAnimation;

	UPROPERTY(Transient, meta = (BindWidgetAnim), BlueprintReadOnly)
		UWidgetAnimation* ToWaitingFadeAnimation;

	UPROPERTY(Transient, meta = (BindWidgetAnim), BlueprintReadOnly)
		UWidgetAnimation* ToOptionFadeAnimation;

	UPROPERTY(Replicated, BlueprintReadOnly)
	class APlayerControlPawn* DuelTarget;
	UDuelOption* DuelTargetOptionWidget;

	virtual void NativeConstruct() override;

	UFUNCTION()
	virtual void IgnoreDuel();

	UFUNCTION()
	virtual void ChallegeDuel();

	void PlayToOptionAnimation();

	UFUNCTION()
		virtual void VisibilityChange(ESlateVisibility InVisibility);

	void SetDuelTarget(APlayerControlPawn* NewDuelTarget);
};
