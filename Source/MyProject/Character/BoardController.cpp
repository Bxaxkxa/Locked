// Fill out your copyright notice in the Description page of Project Settings.


#include "BoardController.h"
#include "LockedCharacter.h"

void ABoardController::BeginPlay()
{
	Super::BeginPlay();

	//SetInputMode(FInputModeUIOnly());
}

void ABoardController::SetupInputComponent()
{
}

void ABoardController::AcknowledgePossession(APawn* PossesedPawn)
{
	Super::AcknowledgePossession(PossesedPawn);

	Player = Cast<ALockedCharacter>(PossesedPawn);
}

ABoardController::ABoardController()
{
}

void ABoardController::Tick(float DeltaTime)
{
}

void ABoardController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
}

void ABoardController::OnUnPossess()
{
	Super::OnUnPossess();
}
