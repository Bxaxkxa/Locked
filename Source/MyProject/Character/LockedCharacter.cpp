// Fill out your copyright notice in the Description page of Project Settings.


#include "LockedCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "MyProject/Room/MainRoomTile.h"
#include "LockedAIController.h"
#include "PlayerControlPawn.h"

// Sets default values
ALockedCharacter::ALockedCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALockedCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentRoom = Cast<AMainRoomTile>(UGameplayStatics::GetActorOfClass(GetWorld(), AMainRoomTile::StaticClass()));

}

// Called every frame
void ALockedCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ALockedCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ALockedCharacter::CharacterMoveTo(ARoomTile* Room)
{
	GetController<ALockedAIController>()->AIMoveToRoom(Room);
	CurrentRoom = Room;
}

void ALockedCharacter::ActiveInput()
{
	ControllerPawn->bStillInMove = false;
}

