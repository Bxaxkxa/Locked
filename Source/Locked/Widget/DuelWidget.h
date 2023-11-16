// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Locked/Struct/DuelData.h"
#include "DuelWidget.generated.h"

class UImage;
class UTextBlock;
class UWidgetSwitcher;

UCLASS()
class LOCKED_API UDuelWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UWidgetSwitcher* DuelLayoutSwitcher;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UWidgetSwitcher* AttackerCardFaceSwitcher;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UImage* AttackerWeaponImage;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UTextBlock* AttackerCardName;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UTextBlock* AttackerCardDescription;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UWidgetSwitcher* DefenderCardFaceSwitcher;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UImage* DefenderWeaponImage;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UTextBlock* DefenderCardName;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UTextBlock* DefenderCardDescription;

	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UImage* AttackerDice;
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UImage* DefenderDice;

	UPROPERTY(Transient, meta = (BindWidgetAnim), BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UWidgetAnimation* AttackerCardFlip;
	UPROPERTY(Transient, meta = (BindWidgetAnim), BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UWidgetAnimation* DefenderCardFlip;

	UPROPERTY(Transient, meta = (BindWidgetAnim), BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UWidgetAnimation* AttackerCardFadeIn;
	UPROPERTY(Transient, meta = (BindWidgetAnim), BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UWidgetAnimation* DefenderCardFadeIn;

	UPROPERTY(BlueprintReadWrite, Replicated, meta = (AllowPrivateAccess = "true"))
		TArray<FDuelData> DuelData;

	UPROPERTY(BlueprintReadWrite, Replicated, meta = (AllowPrivateAccess = "true"))
		FDuelData PlayerDuelData;

	UPROPERTY(BlueprintReadWrite, Replicated, meta = (AllowPrivateAccess = "true"))
		EDuelResult PendingDuelResult;

	UPROPERTY(BlueprintReadWrite, Replicated, meta = (AllowPrivateAccess = "true"))
		bool bIsRollingDice = false;

	UPROPERTY(BlueprintReadWrite, Replicated, meta = (AllowPrivateAccess = "true"))
		bool bIsAttackerCardPlaced = false;
	UPROPERTY(BlueprintReadWrite, Replicated, meta = (AllowPrivateAccess = "true"))
		bool bIsDefenderCardPlaced = false;

	int32 RandomDice = 1;

public:
	void UpdateDuelData(TArray<FDuelData> UpdatedDuelData);

	void PlayDuelAnimation(EDuelResult DuelResult);

	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
		void RollDice();

	UFUNCTION(BlueprintCallable)
		void StopDice();

	void UpdateDiceRoll(FDuelDiceData UpdatedDuelDiceData);
};
