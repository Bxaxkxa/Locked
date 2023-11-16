// Fill out your copyright notice in the Description page of Project Settings.


#include "RoomCount.h"
#include "Components/TextBlock.h"

void URoomCount::UpdateCount(int NewCount)
{
	CountText->SetText(FText::AsNumber(NewCount));
}
