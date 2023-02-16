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

	template<typename T>
	FORCEINLINE static TArray<T> ShuffleArray(TArray<T> ShuffledArray)
	{
			TArray<T> OutArray = ShuffledArray;
			const int32 ShufflesNum = OutArray.Num() - 1;
			for (int32 i = 0; i < ShufflesNum; ++i)
			{
				int32 SwapIndex = FMath::RandRange(i, ShufflesNum);
				OutArray.Swap(i, SwapIndex);
			}
			return OutArray;
	}
};