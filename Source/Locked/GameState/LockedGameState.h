// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Locked/Struct/CardStruct.h"
#include "LockedGameState.generated.h"

/**
 *
 */
UCLASS()
class LOCKED_API ALockedGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<EItemList, FItemCard> ItemCardGenerationArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Deck", Replicated)
		TArray<FItemData> ItemDeck;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Deck", Replicated)
		TArray<FItemData> TreasureDeck;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Deck", Replicated)
		TArray<FItemData> ItemPile;

	ALockedGameState();

	UFUNCTION(Server, Reliable)
		void Server_GenerateDeckCard();

	virtual void OnConstruction(const FTransform& Transform) override;

	bool DrawItem(FItemData& DrawnItem);
	bool DrawTreasure(FItemData& DrawnTreasure);

	void RefillItemDeck();

	UFUNCTION(Server, Reliable, BlueprintCallable)
		void Server_PutItemToPile(FItemData ItemCard);

	void ShuffleItemDeck();

protected:
	virtual void BeginPlay() override;
};
