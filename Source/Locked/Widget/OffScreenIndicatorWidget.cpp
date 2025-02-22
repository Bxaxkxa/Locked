// Fill out your copyright notice in the Description page of Project Settings.


#include "OffScreenIndicatorWidget.h"
#include "Locked/MiscActor/OffScreenIndicator.h"

void UOffScreenIndicatorWidget::AddPlayerInfo(AOffScreenIndicator* NewInfo)
{
	PlayerPosInfo.Add(NewInfo);
}

