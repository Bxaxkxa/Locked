// Fill out your copyright notice in the Description page of Project Settings.


#include "LockedBFL.h"

FString ULockedBFL::GetEnumName(FString EnumString)
{
	FString LeftResult;
	FString RightResult;
	EnumString.Split("::", &LeftResult, &RightResult);

	return RightResult;
}

