// Fill out your copyright notice in the Description page of Project Settings.
#include "LockedFollowCamera.h"

ULockedFollowCamera::ULockedFollowCamera()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void ULockedFollowCamera::BeginPlay()
{
	Super::BeginPlay();
	//SetComponentTickEnabled(false);
}

void ULockedFollowCamera::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TargetTransform.GetLocation().IsZero())
		return;

	SetRelativeLocation(FMath::VInterpTo(GetRelativeLocation(), TargetTransform.GetLocation(), DeltaTime, 10.0f));
	SetRelativeRotation(FMath::RInterpTo(GetRelativeRotation(), FRotator(TargetTransform.GetRotation()), DeltaTime,10.0f));
}

void ULockedFollowCamera::ChangeCameraAngle(FTransform NewAngle)
{
	TargetTransform = NewAngle;
}

