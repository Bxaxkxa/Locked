// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "MyProject/Character/LobbyPlayerController.h"
#include "Kismet/GameplayStatics.h"

void ALobbyGameMode::StartPlay()
{
	Super::StartPlay();
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	BoardGameSettings.AddPlayerNumber();
}
