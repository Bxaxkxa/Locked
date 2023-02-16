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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		int MaxAvailableMove = 3;

	UPROPERTY(Replicated, ReplicatedUsing = Server_ChangePlayerCameraBehaviour)
		EMovementInputState CurrentMovementInputState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
		int Health = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
		int MaxHealth = 2;

	UPROPERTY(Replicated)
		FOnBehaviourChange OnMovementBehaviourChangeDelegate;

	ALockedPlayerState();

	UFUNCTION(Server, Reliable)
		virtual void Server_ChangePlayerCameraBehaviour();

	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void UseMovePoint() { AvailableMove--; }
	void DoubleMovePoint() { AvailableMove *= 2; }
	int GetHealthPoint() { return Health; }

	UFUNCTION(Server, Reliable)
		void Server_RefreshMovePoint();
};
