// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Application/NavigationConfig.h"


class MYPROJECT_API FGameNavigationConfig : public FNavigationConfig
{
public:
	FGameNavigationConfig() : FNavigationConfig(){
	}

	virtual EUINavigationAction GetNavigationActionFromKey(const FKeyEvent& InKeyEvent) const override;
};