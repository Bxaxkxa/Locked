// Fill out your copyright notice in the Description page of Project Settings.


#include "LockedPlayerState.h"
#include "Locked/GameMode/MyProjectGameMode.h"
#include "Net/UnrealNetwork.h"

ALockedPlayerState::ALockedPlayerState()
{
	SetReplicates(true);
}

void ALockedPlayerState::ResetHealthPoint_Implementation()
{
	MaxHealth = 2;
	Health = MaxHealth;

	MaxAvailableMove = 1 + Health;
	AvailableMove = MaxAvailableMove;
}

float ALockedPlayerState::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DamageAmount;
	Health = FMath::Clamp(Health, 0, MaxHealth);

	MaxAvailableMove = 1 + Health;

	return Health;
}

void ALockedPlayerState::UseMovePoint_Implementation()
{
	AvailableMove--;
}

void ALockedPlayerState::DoubleMovePoint_Implementation()
{
	AvailableMove *= 2; 
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

