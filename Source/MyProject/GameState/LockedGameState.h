// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MyProject/Struct/CardStruct.h"
#include "LockedGameState.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ALockedGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TMap<EItemList, FItemCard> ItemCardGenerationArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Deck", Replicated)
	TArray<FItemData> ItemDeck;

	ALockedGameState();

	UFUNCTION(Server,Reliable)
	void Server_GenerateItemDeckCard();

	FItemData DrawItem();

	void ShuffleItemDeck();

protected:
	virtual void BeginPlay() override;
};
