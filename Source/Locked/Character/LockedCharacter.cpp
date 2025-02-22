// Fill out your copyright notice in the Description page of Project Settings.


#include "LockedCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Locked/Room/MainRoomTile.h"
#include "Locked/MiscActor/OffScreenIndicator.h"
#include "LockedAIController.h"
#include "PlayerControlPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ALockedCharacter::ALockedCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetReplicates(true);
	SetReplicateMovement(true);
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

void ALockedCharacter::SpawnPawnIndicatorActor()
{
	UWorld* World = GetWorld();
	AOffScreenIndicator* SpawnedIndicator = World->SpawnActor<AOffScreenIndicator>();
	SpawnedIndicator->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
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
	ControllerPawn->CheckMoveAvailability();

	if (ControllerPawn->RunRoomEvent())
		return;

	ControllerPawn->bStillInMove = false;

	ControllerPawn->CheckMovePoint();
}

void ALockedCharacter::ChangeToIdleCollision(bool State)
{
	State ? GetCapsuleComponent()->SetCollisionProfileName("IgnorePawnOnly") : GetCapsuleComponent()->SetCollisionProfileName("Pawn");
}

void ALockedCharacter::RagDoll_Implementation(bool bState)
{
	USkeletalMeshComponent* SkelMesh = GetMesh();
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();

	SkelMesh->SetSimulatePhysics(bState);

	if (!bState)
	{
		SkelMesh->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		SkelMesh->SetRelativeLocation(FVector(0, 0, -90));
		SkelMesh->SetRelativeRotation(FRotator(0, -90, 0));
	}


	bState ? CapsuleComp->SetCollisionProfileName("Pawn") : CapsuleComp->SetCollisionProfileName("IgnorePawnOnly");
}

void ALockedCharacter::Died()
{
	RagDoll(true);
}

void ALockedCharacter::SetPlayerBackToMainRoom_Implementation()
{
	AActor* MainRoomActor = UGameplayStatics::GetActorOfClass(GetWorld(), AMainRoomTile::StaticClass());
	AMainRoomTile* MainRoom = Cast<AMainRoomTile>(MainRoomActor);

	ControllerPawn->CurrentRoom = MainRoom;

	SetActorLocation(MainRoom->GetActorLocation());
}

void ALockedCharacter::Respawn()
{
	RagDoll(false);

	SetPlayerBackToMainRoom();
}

void ALockedCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}
