// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALobbyCharacter::ALobbyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//PerspectiveCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void ALobbyCharacter::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ALobbyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ALobbyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}