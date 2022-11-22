// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetSwitcher.h"
#include "MyProject/Enum/ActionIndicatorEnum.h"
#include "ActionIndicatorLayout.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UActionIndicatorLayout : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget), BlueprintReadOnly)
		UWidgetSwitcher* LayoutSwitcher;

	UFUNCTION(BlueprintCallable)
	void ChangeIndicatorLayout(EActionLayout ActionLayoutStyle);
};
