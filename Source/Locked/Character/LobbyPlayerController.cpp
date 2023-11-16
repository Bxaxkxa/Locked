// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "Camera/CameraActor.h"
#include "Locked/MiscActor/SharedCamera.h"
#include "Locked/Character/LobbyCharacter.h"
#include "Locked/Widget/ActionIndicatorLayout.h"
#include "Locked/Widget/LoadingScreen.h"
#include "Kismet/GameplayStatics.h"

ALobbyPlayerController::ALobbyPlayerController()
{
	SetReplicates(true);
	SetReplicateMovement(true);
}

void ALobbyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	//SetShowMouseCursor(false);
	//bEnableMouseOverEvents = false;
	//bEnableClickEvents = false;
}

void ALobbyPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);
	PossessedLobbyChar = Cast<ALobbyCharacter>(aPawn);

	AActor* SharedCamera = UGameplayStatics::GetActorOfClass(GetWorld(), ASharedCamera::StaticClass());
	if (GetWorld()->GetFirstPlayerController() == this)
	{
		SetViewTargetWithBlend(SharedCamera, 0.0f);
	}
	SetViewTargetWithBlend(SharedCamera, 1.0f);
}

void ALobbyPlayerController::AcknowledgePossession(APawn* PossesedPawn)
{
	Super::AcknowledgePossession(PossesedPawn);

}

void ALobbyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	//InputComponent->BindAxis("MoveForward", this, &ALobbyPlayerController::MoveForward);
	//InputComponent->BindAxis("MoveRight", this, &ALobbyPlayerController::MoveRight);
}

void ALobbyPlayerController::MoveForward(float Value)
{
	PossessedLobbyChar->MoveForward(Value);
}

void ALobbyPlayerController::MoveRight(float Value)
{
	PossessedLobbyChar->MoveRight(Value);
}

void ALobbyPlayerController::TransitionToGame_Implementation()
{
	ULoadingScreen* TurnDisplay = Cast<ULoadingScreen>(CreateWidget(this, LobbyTransitionWidget));

	if (TurnDisplay)
	{
		TurnDisplay->AddToViewport();

		TurnDisplay->StartFadeInAnimation();
	}
}
