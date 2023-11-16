// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuPlayerController.h"
#include "Locked/Widget/ActionIndicatorLayout.h"

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetShowMouseCursor(false);
	bEnableMouseOverEvents = false;
	bEnableClickEvents = false;
}
