// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Locked/Enum/PlayerMovementState.h"
#include "SelectedButton.h"
#include "PlayerActionWidget.generated.h"

/**
 *
 */
UCLASS()
class LOCKED_API UPlayerActionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
		USelectedButton* MoveActionButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
		USelectedButton* ItemButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
		USelectedButton* ViewMapButton;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
		USelectedButton* EndTurnButton;

	UPROPERTY(BlueprintReadOnly)
		USelectedButton* LastFocusedButton;

	UPROPERTY(Replicated)
		class ABoardController* PlayerOwner;

	UPROPERTY(BlueprintReadOnly, Replicated)
		class ALockedPlayerState* LockedPlayerState;

	FTimerHandle UIDelayTimerHandle;
	float UIDelayTime = 0.2f;

	virtual void NativeConstruct() override;

	UFUNCTION()
		virtual void SwitchToCharacterMoveState();
	UFUNCTION()
		virtual void DelayToSwitchToCharacterMoveState();

	UFUNCTION()
		virtual void SetupPlayerVariable();

	UFUNCTION()
		virtual void EndTurn();
	UFUNCTION()
		virtual void DelayToEndTurn();

	UFUNCTION()
		virtual void SwitchToTileMoveState();

	UFUNCTION()
		virtual void SetItemAsLastFocusedButton();

	UFUNCTION(Client, Reliable)
		void UnFocus();

	UFUNCTION()
		virtual void VisibilityChange(ESlateVisibility InVisibility);
};
