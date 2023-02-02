// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "InputSaveFile.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UInputSaveFile : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
		FKey KeyboardConfirmKey = EKeys::Z;

	UPROPERTY(BlueprintReadWrite)
		FKey GamepadConfirmKey = EKeys::Virtual_Accept;

	UPROPERTY(BlueprintReadWrite)
		FKey KeyboardBackKey = EKeys::X;

	UPROPERTY(BlueprintReadWrite)
		FKey GamepadBackKey = EKeys::Virtual_Back;
};
