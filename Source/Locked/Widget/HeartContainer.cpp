// Fill out your copyright notice in the Description page of Project Settings.


#include "HeartContainer.h"
#include "Components/TextBlock.h"
#include "HeartIcon.h"

void UHeartContainer::SetSteamID(FString SteamID)
{
	PlayerSteamID->SetText(FText::FromString(SteamID));
}

void UHeartContainer::UpdatePlayerHealth(int UpdatedHealthPoint)
{
	for (int i = 0; i < Hearts.Num(); i++)
	{
		Hearts[i]->ActivateHealth(i + 1 <= UpdatedHealthPoint);
	}
}

void UHeartContainer::UpdateItemCount(int UpdatedItemCount)
{
	FString Str = FString::FromInt(UpdatedItemCount);
	ItemCount->SetText(FText::FromString(Str));
}

void UHeartContainer::UpdateTreasureCount(int UpdatedTreasureCount)
{
	FString Str = FString::FromInt(UpdatedTreasureCount);
	TreasureCount->SetText(FText::FromString(Str));
}
