// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MYPROJECT_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	bool bIsAdmin = false;

	UPROPERTY(BlueprintReadWrite)
		class UActionIndicatorLayout* ActionIndicatorWidget;

	ALobbyPlayerController();

	virtual void OnPossess(APawn* aPawn) override;

	virtual void AcknowledgePossession(APawn* PossesedPawn) override;

protected:
	virtual void BeginPlay() override;
};
