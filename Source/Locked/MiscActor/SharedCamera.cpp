// Fill out your copyright notice in the Description page of Project Settings.


#include "SharedCamera.h"
#include "Camera/CameraComponent.h"

// Sets default values
ASharedCamera::ASharedCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a follow camera
	PerspectiveCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	PerspectiveCamera->SetupAttachment(RootComponent); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	PerspectiveCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASharedCamera::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASharedCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

