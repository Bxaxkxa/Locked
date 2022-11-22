// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "SelectedButton.h"
#include "MyProject/Struct/GameSettings.h"
#include "GameSettingWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UGameSettingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
		USelectedButton* ReadyButton;

	FGameSettings* BoardGameSettings = nullptr;

	virtual void NativeConstruct() override;

	UFUNCTION()
	virtual void ReadyUp();
};
