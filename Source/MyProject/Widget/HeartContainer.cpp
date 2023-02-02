// Fill out your copyright notice in the Description page of Project Settings.


#include "HeartContainer.h"
#include "HeartIcon.h"

void UHeartContainer::UpdatePlayerHealth(int UpdatedHealthPoint)
{
	for (int i = 0; i < Hearts.Num(); i++)
	{
		Hearts[i]->ActivateHealth(i + 1 <= UpdatedHealthPoint);
	}
}
