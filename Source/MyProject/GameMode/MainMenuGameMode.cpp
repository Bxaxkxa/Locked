// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"
#include "LockedNavigationInputConfig.h"
#include "Kismet/GameplayStatics.h"
#include "MyProject/SaveFile/InputSaveFile.h"

void AMainMenuGameMode::StartPlay()
{
	Super::StartPlay();

	if (!(UGameplayStatics::LoadGameFromSlot("GameInputSettings", 0)))
	{
		UInputSaveFile* InputSettingSaveFile = Cast<UInputSaveFile>(UGameplayStatics::CreateSaveGameObject(UInputSaveFile::StaticClass()));

		UGameplayStatics::SaveGameToSlot(InputSettingSaveFile, "GameInputSettings", 0);
	}

	FSlateApplication::Get().SetNavigationConfig(MakeShared<FGameNavigationConfig>());
}
