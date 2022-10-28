// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "LockedPlayerState.generated.h"

/**
 *
 */

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnBehaviourChange, EMovementInputState&, NewInputState);
UCLASS()
class MYPROJECT_API ALockedPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
		int AvailableMove = 0;

	UPROPERTY(Replicated, ReplicatedUsing = Server_ChangePlayerCameraBehaviour)
		EMovementInputState CurrentMovementInputState;

	ALockedPlayerState();

	UFUNCTION(Server, Reliable)
		virtual void Server_ChangePlayerCameraBehaviour();

	UPROPERTY(Replicated)
		FOnBehaviourChange OnMovementBehaviourChangeDelegate;

	void UseMovePoint() { AvailableMove--; }

	UFUNCTION(Server, Reliable)
		void Server_RefreshMovePoint();
};
