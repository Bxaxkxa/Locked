// Fill out your copyright notice in the Description page of Project Settings.


#include "LockedGameState.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetArrayLibrary.h"
#include "MyProject/LockedBFL.h"
#include "Net/UnrealNetwork.h"

ALockedGameState::ALockedGameState()
{
	for (uint8 i = static_cast<uint8>(EItemList::Dagger); i < static_cast<uint8>(EItemList::Num); i++)
	{
		FItemCard GeneratedItemCardList = ItemCardGenerationArray.FindOrAdd(EItemList(i));
		GeneratedItemCardList.GetItemData().SetItemName(EItemList(i));
	}
}

void ALockedGameState::Server_GenerateItemDeckCard_Implementation()
{
	for (auto const& [key, val] : ItemCardGenerationArray)
		//for (auto it = ItemCardGenerationArray.begin(); it != ItemCardGenerationArray.end(); it++)
	{
		FItemCard ItemCardValue = val;
		for (int i = 0; i < ItemCardValue.GetCardAmounts(); i++)
		{
			FItemData NewItemCard = FItemData(ItemCardValue.GetItemData(), i);
			ItemDeck.Add(NewItemCard);
		}
	}

	ItemDeck = ULockedBFL::ShuffleArray(ItemDeck);
}

FItemData ALockedGameState::DrawItem()
{
	if (ItemDeck.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(1, 5, FColor::Green, "Deck Is Empty");
		return FItemData();
	}

	int32 RandNumber;

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return FItemData();
	}

	RandNumber = FMath::RandRange(0, ItemDeck.Num() - 1);

	FItemData DrawedItem = ItemDeck[RandNumber];

	ItemDeck.RemoveAt(RandNumber);

	return DrawedItem;
}

void ALockedGameState::Server_PutItemToPile_Implementation(FItemData ItemCard)
{
	ItemPile.Add(ItemCard);
}

void ALockedGameState::ShuffleItemDeck()
{
	const int32 ShufflesNum = ItemDeck.Num() - 1;
	for (int32 i = 0; i < ShufflesNum; ++i)
	{
		int32 SwapIndex = FMath::RandRange(i, ShufflesNum);
		ItemDeck.Swap(i, SwapIndex);
	}
}



void ALockedGameState::BeginPlay()
{
	Super::BeginPlay();

	Server_GenerateItemDeckCard();
}

void ALockedGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALockedGameState, ItemDeck);
	DOREPLIFETIME(ALockedGameState, ItemPile);
}
