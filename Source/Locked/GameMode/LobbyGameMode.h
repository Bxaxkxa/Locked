// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Locked/Struct/GameSettings.h"
#include "LobbyGameMode.generated.h"

/**
 *
 */
UCLASS()
class LOCKED_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		TSubclassOf<class ULoadingScreen> TestingWidget;*/

	FGameSettings BoardGameSettings;

	int32 ReadyPlayerCount = 0;

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* ExitingController) override;

public:
	virtual void StartPlay() override;

	void AddReadyPlayerCount();
	void PlayerUnReady();

	void FadeLoadScreen();
};
