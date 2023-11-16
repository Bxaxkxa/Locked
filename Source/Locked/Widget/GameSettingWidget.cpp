// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSettingWidget.h"
#include "Locked/GameMode/LobbyGameMode.h"
#include "Locked/SaveFile/BoardSettingsSave.h"
#include "Kismet/GameplayStatics.h"


void UGameSettingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//ALobbyGameMode* GameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
	//BoardGameSettings = &GameMode->BoardGameSettings;

	//if (ReadyButton->OnButtonClick.IsBound())
	//	return;

	//ReadyButton->OnButtonClick.AddDynamic(this, &UGameSettingWidget::ReadyUp);
}

void UGameSettingWidget::ReadyUp()
{
	UBoardSettingsSave* GameSettingsSaveFile = Cast<UBoardSettingsSave>(UGameplayStatics::CreateSaveGameObject(UBoardSettingsSave::StaticClass()));

	GameSettingsSaveFile->SetGameSettingsSave(*BoardGameSettings);

	UGameplayStatics::SaveGameToSlot(GameSettingsSaveFile, "BoardGameSettings",0);

	GetWorld()->ServerTravel("LockedLevel");
}
