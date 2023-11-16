// Fill out your copyright notice in the Description page of Project Settings.


#include "HeartIcon.h"

void UHeartIcon::ActivateHealth(bool isBeingActived)
{
	if (bIsActive == isBeingActived)
		return;

	bIsActive = isBeingActived;

	bIsActive ? PlayAnimation(ActivateHealthAnim) : PlayAnimation(DeactivateHealthAnim);
}
