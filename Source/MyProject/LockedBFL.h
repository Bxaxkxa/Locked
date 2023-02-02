// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LockedBFL.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ULockedBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FString GetEnumName(FString EnumString);
};
