// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Locked/Struct/CardStruct.h"
#include "Locked/Enum/ObtainItemMethod.h"
#include "ObtainedItemUI.generated.h"

/**
 *
 */
UCLASS()
class LOCKED_API UObtainedItemUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient, meta = (BindWidgetAnim), BlueprintReadOnly)
		UWidgetAnimation* ObtainedItemAnimation;

	UPROPERTY(BlueprintReadOnly)
		FItemData ItemData;
	UPROPERTY(BlueprintReadOnly)
		EObtainItemMethod PendingObtainMethod;

	void ObtainItem(FItemData NewItemData, EObtainItemMethod ObtainMethod);
};
