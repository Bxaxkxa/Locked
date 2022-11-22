// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"
#include "Camera/CameraActor.h"
#include "MyProject/SharedCamera.h"
#include "MyProject/Widget/ActionIndicatorLayout.h"
#include "Kismet/GameplayStatics.h"

ALobbyPlayerController::ALobbyPlayerController()
{
	SetReplicates(true);
	SetReplicateMovement(true);
}

void ALobbyPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

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

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetShowMouseCursor(false);
	bEnableMouseOverEvents = false;
	bEnableClickEvents = false;
}

