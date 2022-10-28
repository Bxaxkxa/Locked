// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Epic Games, Inc. All Rights Reserved.
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyProject/Struct/GameSettings.h"
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

	UPROPERTY(BlueprintReadWrite)
		FGameSettings BoardGameSettings;

	UPROPERTY(BlueprintReadWrite)
		class UPlayerTurnDisplay* TurnDisplayWidget;

	virtual void StartPlay() override;

	UFUNCTION(BlueprintCallable)
		void NextPlayerTurn();

	UFUNCTION(Server, Reliable)
		void Server_StartInitialTurn();

	void ShowNextTurnDisplay();

	UFUNCTION(BlueprintCallable)
		void AddTurnDisplay(int PlayerTurnIndex);

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
