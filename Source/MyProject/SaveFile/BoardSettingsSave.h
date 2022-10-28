// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MyProject/Struct/GameSettings.h"
#include "BoardSettingsSave.generated.h"

/**
 *
 */
UCLASS()
class MYPROJECT_API UBoardSettingsSave : public USaveGame
{
	GENERATED_BODY()

private:
	UPROPERTY()
		FGameSettings BoardGameSettingSave;

public:
	void SetGameSettingsSave(FGameSettings GameSettings) { BoardGameSettingSave = GameSettings; }

	FGameSettings GetGameSettingsSave() { return BoardGameSettingSave; }
};
