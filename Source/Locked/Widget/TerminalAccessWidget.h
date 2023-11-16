// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TerminalAccessWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOCKED_API UTerminalAccessWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
		class UWidgetSwitcher* AccessIndex;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
		UWidgetAnimation* ShowAccess;

	void ShowAccessAnimation(bool bIsOn, bool bIsServer);
};
