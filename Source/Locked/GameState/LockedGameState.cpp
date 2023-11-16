// Fill out your copyright notice in the Description page of Project Settings.


#include "LockedGameState.h"
#include "Engine/DataTable.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Locked/BlueprintLibrary/LockedBFL.h"
#include "Locked/Character/BoardController.h"
#include "Locked/SaveFile/BoardSettingsSave.h"
#include "Net/UnrealNetwork.h"

ALockedGameState::ALockedGameState()
{
}

void ALockedGameState::Server_GenerateDeckCard_Implementation()
{
	TArray<FItemData>* FilledDeck;

	for (auto const& [key, val] : ItemCardGenerationArray)
	{
		FItemCard ItemCardValue = val;
		FilledDeck = &ItemDeck;

		if (ItemCardValue.GetItemData().bIsTreasure)
			FilledDeck = &TreasureDeck;


		for (int i = 0; i < ItemCardValue.GetCardAmounts(); i++)
		{
			//FItemData NewItemCard = FItemData(ItemCardValue.GetItemData(), i);
			FilledDeck->Emplace(ItemCardValue.GetItemData(), i);
			//FilledDeck->Add(NewItemCard);
		}
	}

	ItemDeck = ULockedBFL::ShuffleArray(ItemDeck);
	TreasureDeck = ULockedBFL::ShuffleArray(TreasureDeck);
}

void ALockedGameState::OnConstruction(const FTransform& Transform)
{
	for (uint8 i = static_cast<uint8>(EItemList::Dagger); i < static_cast<uint8>(EItemList::Num); i++)
	{
		FItemCard GeneratedItemCardList = ItemCardGenerationArray.FindOrAdd(EItemList(i));
		GeneratedItemCardList.GetItemData().SetItemName(EItemList(i));
	}
}

bool ALockedGameState::DrawItem(FItemData& DrawnItem)
{
	if (ItemDeck.IsEmpty())
	{
		RefillItemDeck();
		if (ItemDeck.IsEmpty())
		{
			//Player->Server_EndTurn();
			return false;
		}
	}

	int32 RandNumber;

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		//Player->Server_EndTurn();
		//return;
		return false;
	}
	RandNumber = FMath::RandRange(0, ItemDeck.Num() - 1);
	DrawnItem = ItemDeck[RandNumber];
	ItemDeck.RemoveAt(RandNumber);

	return true;
}

bool ALockedGameState::DrawTreasure(FItemData& DrawnTreasure)
{
	if (TreasureDeck.IsEmpty())
	{
		return false;
	}

	int32 RandNumber;

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return false;
	}

	RandNumber = FMath::RandRange(0, TreasureDeck.Num() - 1);

	DrawnTreasure = TreasureDeck[RandNumber];

	TreasureDeck.RemoveAt(RandNumber);

	return true;
}

void ALockedGameState::RefillItemDeck()
{
	do
	{
		ItemDeck.Add(ItemPile[0]);

		ItemPile.RemoveAt(0);

	} while (!ItemPile.IsEmpty());

	ItemDeck = ULockedBFL::ShuffleArray(ItemDeck);
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

	UBoardSettingsSave* GameSettingsSaveFile = Cast<UBoardSettingsSave>(UGameplayStatics::LoadGameFromSlot("BoardGameSettings", 0));
	TArray<int> SettingItemCardCount = GameSettingsSaveFile->GetGameSettingsSave().GetItemCardCount();

	for (auto const& [key, val] : ItemCardGenerationArray)
	{
		FItemCard ItemCardValue = val;
		EItemList ItemCardKey = key;

		ItemCardGenerationArray[ItemCardKey].SetCardAmounts(SettingItemCardCount[static_cast<int>(ItemCardKey)]);
		//ItemCardValue.SetCardAmounts(SettingItemCardCount[static_cast<int>(ItemCardKey)]);
	}

	Server_GenerateDeckCard();
}

void ALockedGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALockedGameState, ItemDeck);
	DOREPLIFETIME(ALockedGameState, TreasureDeck);
	DOREPLIFETIME(ALockedGameState, ItemPile);
}
