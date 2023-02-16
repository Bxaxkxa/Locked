// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyProject/Enum/ObtainItemMethod.h"
#include "OverCapacityWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API UOverCapacityWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		EObtainItemMethod PendingObtainMethod;
};
