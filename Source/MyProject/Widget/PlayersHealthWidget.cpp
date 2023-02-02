// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayersHealthWidget.h"
#include "HeartContainer.h"

void UPlayersHealthWidget::UpdatePlayerHealth(TArray<int> PlayersHealth)
{
	for (int i = 0; i < PlayersHealth.Num(); i++)
	{
		PlayersHealthContainers[i]->UpdatePlayerHealth(PlayersHealth[i]);
	}
}
