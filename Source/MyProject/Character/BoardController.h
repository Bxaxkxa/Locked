// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BoardController.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ABoardController : public APlayerController
{
	GENERATED_BODY()
	
public:

protected:
    virtual void BeginPlay() override;

    virtual void SetupInputComponent() override;
    virtual void AcknowledgePossession(APawn* PossesedPawn) override;

    class ALockedCharacter* Player;

    UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
    class ARoomTile* CurrentSelectedRoom;

public:
    ABoardController();

    virtual void Tick(float DeltaTime) override;

    virtual void OnPossess(APawn* aPawn) override;
    virtual void OnUnPossess() override;

    void MovePlayerCharacter(ARoomTile* Room);
};
