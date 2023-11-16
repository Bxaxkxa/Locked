// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayersHealthWidget.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "HeartContainer.h"

void UPlayersHealthWidget::SetupPlayerInfo(TArray<FString> PlayerNames)
{
	for (FString it : PlayerNames)
	{
		UHeartContainer* CreatedContainer = CreateWidget<UHeartContainer>(this, ContainerBPClass);
		CreatedContainer->SetSteamID(it);

		UHorizontalBoxSlot* ContainerSlot = Cast<UHorizontalBoxSlot>(ContainerBox->AddChild(CreatedContainer));
		ContainerSlot->SetSize(ESlateSizeRule::Fill);
		//ContainerSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
		ContainerSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
	}
}

void UPlayersHealthWidget::UpdatePlayerHealth(TArray<int> PlayersHealth)
{
	for (int i = 0; i < PlayersHealth.Num(); i++)
	{
		UHeartContainer* ChildContainer = Cast<UHeartContainer>(ContainerBox->GetChildAt(i));
		if (ChildContainer)
		{
			ChildContainer->UpdatePlayerHealth(PlayersHealth[i]);
		}
	}
}

void UPlayersHealthWidget::UpdatePlayerInventory(TArray<int> PlayersItemCount, TArray<int> PlayersTreasureCount)
{
	for (int i = 0; i < ContainerBox->GetChildrenCount(); i++)
	{
		UHeartContainer* ChildContainer = Cast<UHeartContainer>(ContainerBox->GetChildAt(i));
		if (ChildContainer)
		{
			ChildContainer->UpdateItemCount(PlayersItemCount[i]);
			ChildContainer->UpdateTreasureCount(PlayersTreasureCount[i]);
		}
	}
}
