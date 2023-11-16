// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "Locked/Character/LobbyPlayerController.h"
#include "Locked/Widget/LoadingScreen.h"
#include "Locked/SaveFile/BoardSettingsSave.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ALobbyGameMode::StartPlay()
{
	Super::StartPlay();

	UBoardSettingsSave* GameSettingsSaveFile = Cast<UBoardSettingsSave>(UGameplayStatics::LoadGameFromSlot("BoardGameSettings", 0));
	if (!GameSettingsSaveFile)
	{
		GameSettingsSaveFile = Cast<UBoardSettingsSave>(UGameplayStatics::CreateSaveGameObject(UBoardSettingsSave::StaticClass()));
		FGameSettings NewSetting;
		TArray<int> DefaultItemCardCount = {0,3,2,1,0,1,3,3,2,1,0,1,2,3,2,2,3};
		TArray<int> DefaultTileCount = { 20,10,5,10,5 };
		TArray<float> DefaultTileChance = { 40,25,10,20,5 };

		NewSetting.SetItemCardCount(DefaultItemCardCount);
		NewSetting.SetTilesCount(DefaultTileCount);
		NewSetting.SetTilesChance(DefaultTileChance);

		GameSettingsSaveFile->SetGameSettingsSave(NewSetting);

		UGameplayStatics::SaveGameToSlot(GameSettingsSaveFile, "BoardGameSettings", 0);
	}
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	BoardGameSettings.AddPlayerNumber();
}

void ALobbyGameMode::Logout(AController* ExitingController)
{
	Super::Logout(ExitingController);

	if (ReadyPlayerCount >= GetNumPlayers())
	{
		GetWorld()->ServerTravel("LockedLevel");
	}
}

void ALobbyGameMode::AddReadyPlayerCount()
{
	ReadyPlayerCount++;

	if (ReadyPlayerCount >= GetNumPlayers())
	{
		FadeLoadScreen();
	}
}

void ALobbyGameMode::PlayerUnReady()
{
	ReadyPlayerCount--;
}

void ALobbyGameMode::FadeLoadScreen()
{
	for (int i = 0; i < GetNumPlayers(); i++)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), i);

		ALobbyPlayerController* LobbyController = Cast<ALobbyPlayerController>(PlayerController);
		if (LobbyController)
		{
			LobbyController->TransitionToGame();
		}
	}
}
