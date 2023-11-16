// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Locked/Struct/GameSettings.h"
#include "Locked/Struct/DuelData.h"
#include "Framework/Application/NavigationConfig.h"
#include "MyProjectGameMode.generated.h"

UCLASS(minimalapi)
class AMyProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyProjectGameMode();

	bool bIsGameStarted = false;
	UPROPERTY(BlueprintReadOnly)
		bool bIsReadyToStart = false;

	UPROPERTY(EditAnywhere, Category = "Debug");
		bool bIsInDebug = false;

	UPROPERTY(Replicated)
		int NumberOfPlayer = 0;

	int PlayerFinishedTravelCount = 0;

	UPROPERTY(Replicated)
		int CurrentPlayerTurn = 0;

	UPROPERTY(BlueprintReadWrite)
		FGameSettings BoardGameSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class ULoadingScreen> LoadingScreenWidgetClass;

	UPROPERTY(BlueprintReadWrite)
		class UPlayerTurnDisplay* TurnDisplayWidget;

	UPROPERTY(Replicated)
		class ABoardController* DuelAttacker;
	UPROPERTY(Replicated)
		class ABoardController* DuelDefender;

	TArray<APlayerController*> PlayersController;

	EDuelResult PendingDuelResult;

	UPROPERTY(BlueprintReadWrite, Replicated)
		TArray<FDuelData> DuelData;

	UPROPERTY(BlueprintReadWrite, Replicated)
		FDuelDiceData DuelDiceData;

	FTimerHandle GameStartDelayTimerHandle;
	float GameStartDelayTime = 0.5f;

	FTimerHandle DuelStartDelayTimerHandle;
	float DuelStartDelayTime = 2.0f;

	virtual void StartPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

	UFUNCTION()
	void RemoveLoadingScreen();

	UFUNCTION(BlueprintCallable)
		void NextPlayerTurn();

	UFUNCTION(BlueprintCallable, Server, Reliable)
		void Server_StartInitialTurn();

	void ShowNextTurnDisplay();

	UFUNCTION(BlueprintCallable)
		void AddTurnDisplay(int PlayerTurnIndex);

	void DuelSetup(ABoardController* Attacker, ABoardController* Defender);

	UFUNCTION(BlueprintCallable)
		void SwitchDuelState();

	void QueuUpWeapon(FDuelData QueuDuelData);
	void UnQueuUpWeapon(bool IsPlayerAttacker);

	void RefreshDuelData();

	void CheckDuelCondition();

	void StopDuel(bool RecheckDuelTarget);

	void StartDuel();

	void UpdateAttackerDuelDiceData(bool IsStillRolling, int32 DiceNumber);
	void UpdateDefenderDuelDiceData(bool IsStillRolling, int32 DiceNumber);

	void UpdateDuelDiceUI();

	void CheckDuelDiceResult();

	void StartPlayerStealCard(EDuelResult DuelResult);
	void StealOpponentCard(FItemData StolenItem);

	void RunPendingDuelResult();

	void SetPlayerHealthInfo();
	void UpdatePlayerHealthInfo();

	void UpdatePlayerInventoryInfo();

	int CalculatePlayerUpdatedTreasure(TArray<FItemData> PlayerInventory);

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
};
