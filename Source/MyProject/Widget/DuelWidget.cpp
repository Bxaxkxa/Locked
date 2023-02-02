// Fill out your copyright notice in the Description page of Project Settings.


#include "DuelWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "MyProject/Struct/CardStruct.h"
#include "MyProject/LockedBFL.h"
#include "MyProject/Character/BoardController.h"
#include "Kismet/KismetStringLibrary.h"
#include "Net/UnrealNetwork.h"


void UDuelWidget::UpdateDuelData(TArray<FDuelData> UpdatedDuelData)
{
	DuelData = UpdatedDuelData;

	FDuelData* AttackerDuelData = DuelData.FindByKey(true);
	FDuelData* DefenderDuelData = DuelData.FindByKey(false);

	//Set attacker card item image
	AttackerWeaponImage->SetBrush(AttackerDuelData->ItemData.ItemImage);
	//Set attacker card name

	//FString EnumString = UKismetStringLibrary::Conv_ByteToString(AttackerDuelData->ItemData.ItemName);
	FString EnumString = UEnum::GetValueAsString(AttackerDuelData->ItemData.ItemName);
	FText CardName = FText::FromString(ULockedBFL::GetEnumName(EnumString));
	AttackerCardName->SetText(CardName);
	//Set attacker card description
	FText CardDesc = FText::FromString(AttackerDuelData->ItemData.ItemDescription);
	AttackerCardDescription->SetText(CardDesc);

	//Set defender card item image
	DefenderWeaponImage->SetBrush(DefenderDuelData->ItemData.ItemImage);
	//Set defender card item image
	//EnumString = UKismetStringLibrary::Conv_ByteToString(uint8(DefenderDuelData->ItemData.ItemName));
	EnumString = UEnum::GetValueAsString(DefenderDuelData->ItemData.ItemName);
	CardName = FText::FromString(ULockedBFL::GetEnumName(EnumString));
	DefenderCardName->SetText(CardName);
	//Set defender card description
	CardDesc = FText::FromString(DefenderDuelData->ItemData.ItemDescription);
	DefenderCardDescription->SetText(CardDesc);


	if (AttackerDuelData->IsReady && !bIsAttackerCardPlaced)
		PlayAnimation(AttackerCardFadeIn);
	else if (!AttackerDuelData->IsReady && bIsDefenderCardPlaced)
		PlayAnimationReverse(AttackerCardFadeIn);

	if (DefenderDuelData->IsReady && !bIsDefenderCardPlaced)
		PlayAnimation(DefenderCardFadeIn);
	else if (!DefenderDuelData->IsReady && bIsDefenderCardPlaced)
		PlayAnimationReverse(DefenderCardFadeIn);
}

void UDuelWidget::PlayDuelAnimation(EDuelResult DuelResult)
{
	PendingDuelResult = DuelResult;

	ABoardController* Owner = GetOwningPlayer<ABoardController>();
	if (Owner)
		Owner->IsPlayerTheDuelAttacker() ? PlayAnimation(DefenderCardFlip) : PlayAnimation(AttackerCardFlip);
}

void UDuelWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	Super::NativeTick(MyGeometry, DeltaTime);

	if (bIsRollingDice)
	{
		RandomDice = FMath::RandRange(1, 6);
	}
}

void UDuelWidget::RollDice()
{
	bIsRollingDice = true;

	AttackerDice->SetVisibility(ESlateVisibility::HitTestInvisible);
	DefenderDice->SetVisibility(ESlateVisibility::HitTestInvisible);

	ABoardController* Owner = GetOwningPlayer<ABoardController>();
	if (Owner)
	{
		Owner->Server_UpdateDuelDiceState(bIsRollingDice, 0);
		Owner->Client_ChangeIndicatorLayout(EActionLayout::DiceRoll);
	}
}

void UDuelWidget::StopDice()
{
	bIsRollingDice = false;

	ABoardController* Owner = GetOwningPlayer<ABoardController>();
	if (Owner)
	{
		Owner->Server_UpdateDuelDiceState(bIsRollingDice, RandomDice);
	}
}

void UDuelWidget::UpdateDiceRoll(FDuelDiceData UpdatedDuelDiceData)
{
	AttackerDice->GetDynamicMaterial()->SetScalarParameterValue("IsRolling", UpdatedDuelDiceData.IsAttackerStillRolling);
	DefenderDice->GetDynamicMaterial()->SetScalarParameterValue("IsRolling", UpdatedDuelDiceData.IsDefenderStillRolling);

	AttackerDice->GetDynamicMaterial()->SetScalarParameterValue("Position", UpdatedDuelDiceData.AttackerDiceNumber);
	DefenderDice->GetDynamicMaterial()->SetScalarParameterValue("Position", UpdatedDuelDiceData.DefenderDiceNumber);
}

void UDuelWidget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDuelWidget, DuelData);
	DOREPLIFETIME(UDuelWidget, PlayerDuelData);
	DOREPLIFETIME(UDuelWidget, PendingDuelResult);
	DOREPLIFETIME(UDuelWidget, bIsRollingDice);
}
