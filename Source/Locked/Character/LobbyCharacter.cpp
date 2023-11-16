// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Locked/LobbyElement/LobbyInteractableBase.h"
#include "Locked/GameMode/LobbyGameMode.h"
#include "Locked/Widget/GameSettingWidget.h"
#include "Net/UnrealNetwork.h"

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

	SearchForNearestInterable();
}

// Called to bind functionality to input
void ALobbyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MoveForward", this, &ALobbyCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ALobbyCharacter::MoveRight);

	InputComponent->BindAction("UnReady", IE_Pressed, this, &ALobbyCharacter::UnReady);
	InputComponent->BindAction("Interact", IE_Pressed, this, &ALobbyCharacter::InteractWithNearestObject);
}

void ALobbyCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.0f)
	{
		// Find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);
	}
}

void ALobbyCharacter::MoveRight(float Value)
{
	if (Controller && Value != 0.0f)
	{
		// Find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// Get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// Add movement in that direction

		AddMovementInput(Direction, Value);
	}
}

void ALobbyCharacter::InteractWithNearestObject()
{
	if (NearestInteractable.InteractableObject)
	{
		NearestInteractable.InteractableObject->Interact(this);
	}
}

void ALobbyCharacter::OpenGameSettingMenu_Implementation()
{
	UGameSettingWidget* GameSetting = Cast<UGameSettingWidget>(CreateWidget(GetController<APlayerController>(), GameSettingWidgetClass));
	if (GameSetting)
	{
		GetController<APlayerController>()->SetInputMode(FInputModeUIOnly());
		GameSetting->AddToViewport(5);
		GameSetting->SetFocus();
	}
}

void ALobbyCharacter::SearchForNearestInterable_Implementation()
{
	UWorld* World = GetWorld();
	
	TArray<AActor*> OutArray;

	UGameplayStatics::GetAllActorsOfClass(World, ALobbyInteractableBase::StaticClass(), OutArray);

	OldClosest = NearestInteractable.InteractableObject;

	NearestInteractable = FClosestObject();

	for (AActor* it : OutArray)
	{
		float NewDistance = FVector::Dist2D(it->GetActorLocation(), GetActorLocation());

		if (NewDistance > 200.0f)
		{
			continue;
		}

		if (!NearestInteractable.InteractableObject || NearestInteractable.Distance > NewDistance)
		{
			NearestInteractable.InteractableObject = Cast<ALobbyInteractableBase>(it);
			NearestInteractable.Distance = NewDistance;
		}
	}
	
	if (OldClosest != NearestInteractable.InteractableObject)
	{
		if (NearestInteractable.InteractableObject)
		{
			//if ((GetLocalRole() == ROLE_AutonomousProxy && !World->IsServer()) || (GetLocalRole() == ROLE_Authority && World->IsServer()))
			if ((GetLocalRole() == ROLE_AutonomousProxy && IsNetMode(ENetMode::NM_Client)) || (GetLocalRole() == ROLE_Authority && IsNetMode(ENetMode::NM_ListenServer)))
				NearestInteractable.InteractableObject->ShowInteractIndicator(true);
		}
		
		if (OldClosest)
		{
			if ((GetLocalRole() == ROLE_AutonomousProxy && IsNetMode(ENetMode::NM_Client)) || (GetLocalRole() == ROLE_Authority && IsNetMode(ENetMode::NM_ListenServer)))
			//if ((GetLocalRole() == ROLE_AutonomousProxy && !World->IsServer()) || (GetLocalRole() == ROLE_Authority && World->IsServer()))
				OldClosest->ShowInteractIndicator(false);
		}
	}
}

void ALobbyCharacter::ReadyUp_Implementation()
{
	if (IsReady)
		return;
	ALobbyGameMode* GameMode = GetWorld()->GetAuthGameMode<ALobbyGameMode>();

	if (GameMode)
		GameMode->AddReadyPlayerCount();

	IsReady = true;

	FVector ReadyLocation = GetActorLocation() + FVector(0, 0, 1200.0f);

	SetActorLocation(ReadyLocation);
}

void ALobbyCharacter::UnReady_Implementation()
{
	if (!IsReady)
		return;

	ALobbyGameMode* GameMode = GetWorld()->GetAuthGameMode<ALobbyGameMode>();

	if (GameMode)
		GameMode->PlayerUnReady();

	IsReady = false;

	FVector ReadyLocation = GetActorLocation() - FVector(0, 0, 1200.0f);

	SetActorLocation(ReadyLocation);
}

void ALobbyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyCharacter, NearestInteractable);
	DOREPLIFETIME(ALobbyCharacter, OldClosest);
}
