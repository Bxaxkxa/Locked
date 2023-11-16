// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

/**
 *
 */
UCLASS()
class LOCKED_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
		bool bIsAdmin = false;

	UPROPERTY(BlueprintReadWrite)
		class UActionIndicatorLayout* ActionIndicatorWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<class ULoadingScreen> LobbyTransitionWidget;

	class ALobbyCharacter* PossessedLobbyChar;

	ALobbyPlayerController();

	virtual void OnPossess(APawn* aPawn) override;

	virtual void AcknowledgePossession(APawn* PossesedPawn) override;

	virtual void SetupInputComponent() override;

	void MoveForward(float Value);
	void MoveRight(float Value);

	UFUNCTION(Client, Reliable)
		void TransitionToGame();

	
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
};
