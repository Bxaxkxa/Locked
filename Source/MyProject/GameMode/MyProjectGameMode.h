// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyProject/Struct/GameSettings.h"
#include "MyProject/Struct/DuelData.h"
#include "Framework/Application/NavigationConfig.h"
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

	UPROPERTY(Replicated)
		class ABoardController* DuelAttacker;
	UPROPERTY(Replicated)
		class ABoardController* DuelDefender;

	TArray<APlayerController*> PlayersController;

	UPROPERTY(BlueprintReadWrite, Replicated)
		TArray<FDuelData> DuelData;

	UPROPERTY(BlueprintReadWrite, Replicated)
		FDuelDiceData DuelDiceData;

	FTimerHandle DuelStartDelayTimerHandle;
	float DuelStartDelayTime = 2.0f;

	virtual void StartPlay() override;

	UFUNCTION(BlueprintCallable)
		void NextPlayerTurn();

	UFUNCTION(Server, Reliable)
		void Server_StartInitialTurn();

	void ShowNextTurnDisplay();

	UFUNCTION(BlueprintCallable)
		void AddTurnDisplay(int PlayerTurnIndex);

	void DuelSetup(ABoardController* Attacker, ABoardController* Defender);

	UFUNCTION(BlueprintCallable)
		void SwitchDuelState();

	void QueuUpWeapon(FDuelData QueuDuelData);
	void UnQueuUpWeapon(bool IsPlayerAttacker);

	void CheckDuelCondition();

	void StopDuel(bool RecheckDuelTarget);

	void StartDuel();

	void UpdateAttackerDuelDiceData(bool IsStillRolling, int32 DiceNumber);
	void UpdateDefenderDuelDiceData(bool IsStillRolling, int32 DiceNumber);

	void UpdateDuelDiceUI();

	void CheckDuelDiceResult();

	void UpdatePlayerHealthInfo();

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
