// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyProjectGameMode.generated.h"

UCLASS(minimalapi)
class AMyProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyProjectGameMode();

	UPROPERTY(Replicated)
		int NumberOfplayer = 0;

	UPROPERTY(Replicated)
		int CurrentPlayerTurn = 0;

	virtual void StartPlay() override;

	void NextPlayerTurn();

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
};



