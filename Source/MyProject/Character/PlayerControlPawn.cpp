// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControlPawn.h"
#include "LockedCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MyProject/Room/MainRoomTile.h"

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

		////FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::FAttachmentTransformRules(EAttachmentRule;
		//FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::FAttachmentTransformRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false);
		//AttachToActor(ControlledCharacter, AttachmentRules);

		ControlledCharacter->ControllerPawn = this;
	}
}

// Called when the game starts or when spawned
void APlayerControlPawn::BeginPlay()
{
	Super::BeginPlay();

	CurrentRoom = Cast<AMainRoomTile>(UGameplayStatics::GetActorOfClass(GetWorld(), AMainRoomTile::StaticClass()));

	SpawnControlledCharacter();

	TargetCameraLocation = GetActorLocation();
}

// Called every frame
void APlayerControlPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector ResultTarget;
	if (bIsFollowBehaviour && ControlledCharacter)
	{
		ResultTarget = FMath::Lerp<FVector>(GetActorLocation(), ControlledCharacter->GetActorLocation(), DeltaTime * CameraFollowSpeed);

		SetActorLocation(ResultTarget);
		return;
	}
	ResultTarget = FMath::Lerp<FVector>(GetActorLocation(), TargetCameraLocation, DeltaTime * CameraFollowSpeed);
	SetActorLocation(ResultTarget);
}

// Called to bind functionality to input
void APlayerControlPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction<FTestingDelegate, APlayerControlPawn, FString>("Left", IE_Pressed, this, &APlayerControlPawn::MoveCharacterTo, "NextLeft");
	PlayerInputComponent->BindAction<FTestingDelegate, APlayerControlPawn, FString>("Right", IE_Pressed, this, &APlayerControlPawn::MoveCharacterTo, "NextRight");
	PlayerInputComponent->BindAction<FTestingDelegate, APlayerControlPawn, FString>("Up", IE_Pressed, this, &APlayerControlPawn::MoveCharacterTo, "NextUp");
	PlayerInputComponent->BindAction<FTestingDelegate, APlayerControlPawn, FString>("Down", IE_Pressed, this, &APlayerControlPawn::MoveCharacterTo, "NextDown");

	PlayerInputComponent->BindAction("ChangeBehaviour", IE_Pressed, this, &APlayerControlPawn::ChangeCameraBehaviour);
}

void APlayerControlPawn::MoveCharacterTo(FString MoveToDirection)
{
	ARoomTile* TargetRoom = CurrentRoom->NeighbourRoom[MoveToDirection];
	if (!TargetRoom)
		return;

	if (!bIsFollowBehaviour)
	{
		MoveCameraTo(TargetRoom);
		return;
	}

	if (!bStillInMove)
	{
		bStillInMove = true;

		ControlledCharacter->CharacterMoveTo(TargetRoom);

		CurrentRoom = TargetRoom;
	}
}

void APlayerControlPawn::MoveCameraTo(ARoomTile* MoveToDirection)
{
	TargetCameraLocation = MoveToDirection->GetActorLocation();
	CurrentRoom = MoveToDirection;
	/*if (MoveToDirection == "NextLeft")
	{
		TargetCameraLocation = GetActorLocation() + FVector(0.0f, -400.0f, 0.0f);
	}
	else if (MoveToDirection == "NextRight")
	{
		TargetCameraLocation = GetActorLocation() + FVector(0.0f, 400.0f, 0.0f);
	}
	else if (MoveToDirection == "NextUp")
	{
		TargetCameraLocation = GetActorLocation() + FVector(400.0f, 0.0f, 0.0f);
	}
	else if (MoveToDirection == "NextDown")
	{
		TargetCameraLocation = GetActorLocation() + FVector(-400.0f, 0.0f, 0.0f);
	}*/
}

void APlayerControlPawn::ChangeCameraBehaviour()
{
	bIsFollowBehaviour = !bIsFollowBehaviour;
	CurrentRoom = ControlledCharacter->CurrentRoom;
	TargetCameraLocation = CurrentRoom->GetActorLocation();
}

