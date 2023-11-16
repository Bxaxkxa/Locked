// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Locked/Enum/DirectionEnum.h"
#include "RoomTileWallMesh.generated.h"

/**
 * 
 */
UCLASS(Meta = (BlueprintSpawnableComponent))
class LOCKED_API URoomTileWallMesh : public UStaticMeshComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere)
		ETileDirection WallDirection;

	void Break(bool bIsBreak);
	bool IsDoorOfDirection(ETileDirection CheckDirection);
};
