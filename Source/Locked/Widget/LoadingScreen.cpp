// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreen.h"

void ULoadingScreen::StartFadeOffAnimation()
{
	PlayAnimation(LoadingFadeOffAnimation);
}

void ULoadingScreen::StartFadeInAnimation()
{
	PlayAnimation(LoadingFadeInAnimation);
}