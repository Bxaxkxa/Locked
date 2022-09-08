// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControlPawn.h"
#include "LockedCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyProject/Room/MainRoomTile.h"
#include "MyProject/RandomGenRoom.h"

// Sets default values
APlayerControlPawn::APlayerControlPawn()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("CameraBoom"));
	SetRootComponent(RootScene);

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(RootScene); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void APlayerControlPawn::SpawnControlledCharacter()
{
	if (CharacterTemplate)
	{
		UWorld* World = GetOwner()->GetWorld();
		if (World == nullptr)
		{
			return;
		}

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = Cast<APawn>(GetOwner());
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		FTransform spawnlocation = GetActorTransform();
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = FRotator::ZeroRotator;

		ControlledCharacter = World->SpawnActor<ALockedCharacter>(CharacterTemplate, SpawnLocation, SpawnRotation, SpawnParams);

		SetActorLocation(CurrentRoom->GetActorLocation());

		ControlledCharacter->ControllerPawn = this;
	}
}

// Called when the game starts or when spawned
void APlayerControlPawn::BeginPlay()
{
	Super::BeginPlay();

	CurrentRoom = Cast<AMainRoomTile>(UGameplayStatics::GetActorOfClass(GetWorld(), AMainRoomTile::StaticClass()));

	SpawnControlledCharacter();

	FollowTarget = ControlledCharacter;

	RandomRoomGenDeck = Cast<ARandomGenRoom>(UGameplayStatics::GetActorOfClass(GetWorld(), ARandomGenRoom::StaticClass()));
}

// Called every frame
void APlayerControlPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector ResultTarget;

	ResultTarget = FMath::Lerp<FVector>(GetActorLocation(), FollowTarget->GetActorLocation(), DeltaTime * CameraFollowSpeed);

	SetActorLocation(ResultTarget);
}

// Called to bind functionality to input
void APlayerControlPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction<FTestingDelegate, APlayerControlPawn, ETileDirection>("Left", IE_Pressed, this, &APlayerControlPawn::ManageMovement, ETileDirection::NextLeft);
	PlayerInputComponent->BindAction<FTestingDelegate, APlayerControlPawn, ETileDirection>("Right", IE_Pressed, this, &APlayerControlPawn::ManageMovement, ETileDirection::NextRight);
	PlayerInputComponent->BindAction<FTestingDelegate, APlayerControlPawn, ETileDirection>("Up", IE_Pressed, this, &APlayerControlPawn::ManageMovement, ETileDirection::NextUp);
	PlayerInputComponent->BindAction<FTestingDelegate, APlayerControlPawn, ETileDirection>("Down", IE_Pressed, this, &APlayerControlPawn::ManageMovement, ETileDirection::NextDown);

	PlayerInputComponent->BindAction("ChangeBehaviour", IE_Pressed, this, &APlayerControlPawn::ChangeCameraBehaviour);

	PlayerInputComponent->BindAction("PlaceTile", IE_Pressed, this, &APlayerControlPawn::PlaceRoomTile);

	PlayerInputComponent->BindAction("DrawTile", IE_Pressed, this, &APlayerControlPawn::DrawRoomTile);
	PlayerInputComponent->BindAction("RotateTile", IE_Pressed, this, &APlayerControlPawn::RotateRoomTile);
}

void APlayerControlPawn::ManageMovement(ETileDirection MoveToDirection)
{
	ARoomTile* TargetRoom = CurrentRoom->NeighbourRoom[static_cast<int>(MoveToDirection)].GetNextRoom();

	switch (CurrentMovementInputState)
	{
	case EMovementInputState::E_CharMovement:
		MoveCharacterTo(TargetRoom);
		break;
	case EMovementInputState::E_TileMovement:
		MoveCameraTo(TargetRoom);
		break;
	case EMovementInputState::E_TilePlacement:
		MovePlacedTile(CurrentRoom->NeighbourRoom[static_cast<int>(MoveToDirection)], MoveToDirection);
		break;
	default:
		break;
	}
}

void APlayerControlPawn::MoveCameraTo(ARoomTile* MoveToTile)
{
	if (!MoveToTile)
		return;

	FollowTarget = MoveToTile;
	CurrentRoom = MoveToTile;
}

void APlayerControlPawn::MoveCharacterTo(ARoomTile* MoveToTile)
{
	if (!bStillInMove && MoveToTile)
	{
		bStillInMove = true;
		ControlledCharacter->CharacterMoveTo(MoveToTile);

		CurrentRoom = MoveToTile;
	}
}

void APlayerControlPawn::MovePlacedTile(FDoorWay DoorWay, ETileDirection MoveToDirection)
{
	if (DoorWay.IsThereDoorway() && !DoorWay.GetNextRoom())
	{
		bool IsGoingLeft = MoveToDirection == ETileDirection::NextLeft;
		bool IsGoingRight = MoveToDirection == ETileDirection::NextRight;
		bool IsGoingUp = MoveToDirection == ETileDirection::NextUp;
		bool IsGoingDown = MoveToDirection == ETileDirection::NextDown;
		FVector AddPosition = FVector((IsGoingUp * 400.0f) + (IsGoingDown * -400.0f), (IsGoingRight * 400.0f) + (IsGoingLeft * -400.0f), 0.0f);
		DrawedRoomTile->SetActorLocation(CurrentRoom->GetActorLocation() + AddPosition);

		LastPlacedDirection = MoveToDirection;
		DrawedRoomTile->CheckConnectionAvailibility(LastPlacedDirection);
		/*	switch (MoveToDirection)
			{
			case ETileDirection::NextLeft:
				break;
			case ETileDirection::NextRight:
				DrawedRoomTile->SetActorLocation(CurrentRoom->GetActorLocation() + FVector(0.0f, 400.0f, 0.0f));
				break;
			case ETileDirection::NextUp:
				DrawedRoomTile->SetActorLocation(CurrentRoom->GetActorLocation() + FVector(400.0f, 0.0f, 0.0f));
				break;
			case ETileDirection::NextDown:
				DrawedRoomTile->SetActorLocation(CurrentRoom->GetActorLocation() + FVector(-400.0f, 0.0f, 0.0f));
				break;
			}*/
	}

}

void APlayerControlPawn::ChangeCameraBehaviour()
{
	if (bStillInMove)
		return;

	switch (CurrentMovementInputState)
	{
	case EMovementInputState::E_CharMovement:
		CurrentMovementInputState = EMovementInputState::E_TileMovement;
		CurrentRoom = ControlledCharacter->CurrentRoom;
		FollowTarget = CurrentRoom;
		break;
	case EMovementInputState::E_TileMovement:
		CurrentMovementInputState = EMovementInputState::E_CharMovement;
		FollowTarget = ControlledCharacter;
		break;
	case EMovementInputState::E_TilePlacement:
		break;
	default:
		break;
	}
}

void APlayerControlPawn::DrawRoomTile()
{
	if (!(CurrentMovementInputState == EMovementInputState::E_CharMovement))
	{
		return;
	}

	FollowTarget = DrawedRoomTile = RandomRoomGenDeck->DrawTile();

	for (int i = 0; i < static_cast<int>(ETileDirection::NUM); i++)
	{
		if (CurrentRoom->NeighbourRoom[i].IsThereDoorway() && !CurrentRoom->NeighbourRoom[i].GetNextRoom())
		{
			bool IsGoingLeft = ETileDirection(i) == ETileDirection::NextLeft;
			bool IsGoingRight = ETileDirection(i) == ETileDirection::NextRight;
			bool IsGoingUp = ETileDirection(i) == ETileDirection::NextUp;
			bool IsGoingDown = ETileDirection(i) == ETileDirection::NextDown;
			FVector AddPosition = FVector((IsGoingUp * 400.0f) + (IsGoingDown * -400.0f), (IsGoingRight * 400.0f) + (IsGoingLeft * -400.0f), 0.0f);
			DrawedRoomTile->SetActorLocation(CurrentRoom->GetActorLocation() + AddPosition);
			LastPlacedDirection = ETileDirection(i);
			DrawedRoomTile->CheckConnectionAvailibility(LastPlacedDirection);
		}
	}
	CurrentMovementInputState = EMovementInputState::E_TilePlacement;
}

void APlayerControlPawn::PlaceRoomTile()
{
	if (!DrawedRoomTile)
	{
		return;
	}

	CurrentRoom->NeighbourRoom[static_cast<int>(LastPlacedDirection)].SetNextRoom(DrawedRoomTile);

	DrawedRoomTile->CheckNeightbourRooms();

	MoveCharacterTo(DrawedRoomTile);

	DrawedRoomTile = nullptr;

	FollowTarget = ControlledCharacter;

	CurrentMovementInputState = EMovementInputState::E_CharMovement;
}

void APlayerControlPawn::RotateRoomTile()
{
	if (DrawedRoomTile)
	{
		DrawedRoomTile->RotateRoomPlacement();
		DrawedRoomTile->CheckConnectionAvailibility(LastPlacedDirection);
	}
}

