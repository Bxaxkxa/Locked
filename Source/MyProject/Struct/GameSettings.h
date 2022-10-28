// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSettings.generated.h"

USTRUCT(BlueprintType)
struct FGameSettings
{
	GENERATED_BODY();
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		int PlayerNumber;

public:
	void SetPlayerNumber(int Num) { PlayerNumber = Num; }
	void AddPlayerNumber() { PlayerNumber += 1; }

	int GetPlayerNumber() { return PlayerNumber; }
};