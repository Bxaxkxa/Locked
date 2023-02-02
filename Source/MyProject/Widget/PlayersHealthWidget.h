// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayersHealthWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UPlayersHealthWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
		TArray<class UHeartContainer*> PlayersHealthContainers;
	
	void UpdatePlayerHealth(TArray<int> PlayersHealth);

};
