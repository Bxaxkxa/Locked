// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Locked/Struct/GameSettings.h"
#include "BoardSettingsSave.generated.h"

/**
 *
 */
UCLASS()
class LOCKED_API UBoardSettingsSave : public USaveGame
{
	GENERATED_BODY()

private:
	UPROPERTY( BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FGameSettings BoardGameSettingSave;

public:
	void SetGameSettingsSave(FGameSettings GameSettings) { BoardGameSettingSave = GameSettings; }

	FGameSettings GetGameSettingsSave() { return BoardGameSettingSave; }
};
