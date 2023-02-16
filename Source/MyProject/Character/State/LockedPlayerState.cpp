// Fill out your copyright notice in the Description page of Project Settings.


#include "LockedPlayerState.h"
#include "MyProject/GameMode/MyProjectGameMode.h"
#include "Net/UnrealNetwork.h"

ALockedPlayerState::ALockedPlayerState()
{
	SetReplicates(true);
}

float ALockedPlayerState::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DamageAmount;

	AMyProjectGameMode* GameMode = Cast<AMyProjectGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->UpdatePlayerHealthInfo();
	if (Health == 1)
	{
		MaxAvailableMove = 2;
	}

	if (Health)
	{
		//TO DO: Death
	}

	return Health;
}

void ALockedPlayerState::Server_ChangePlayerCameraBehaviour_Implementation()
{
	OnMovementBehaviourChangeDelegate.Execute(CurrentMovementInputState);
}

void ALockedPlayerState::Server_RefreshMovePoint_Implementation()
{
	AvailableMove = MaxAvailableMove;
}

void ALockedPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALockedPlayerState, AvailableMove);
	DOREPLIFETIME(ALockedPlayerState, MaxAvailableMove);
	DOREPLIFETIME(ALockedPlayerState, Health);
	DOREPLIFETIME(ALockedPlayerState, MaxHealth);
	DOREPLIFETIME(ALockedPlayerState, OnMovementBehaviourChangeDelegate);
}

