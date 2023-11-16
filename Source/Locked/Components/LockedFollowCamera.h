// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "LockedFollowCamera.generated.h"

/**
 * 
 */
UCLASS()
class LOCKED_API ULockedFollowCamera : public UCameraComponent
{
	GENERATED_BODY()
	
public:
	FTransform TargetTransform;
	FTransform StartTransform;

	float LerpDuration = 0.5f;
	float TimeElapsed = 0.0f;

	ULockedFollowCamera();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ChangeCameraAngle(FTransform NewAngle);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

};
