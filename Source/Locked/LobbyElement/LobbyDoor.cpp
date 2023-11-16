// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyDoor.h"
#include "Locked/Character/LobbyCharacter.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ALobbyDoor::ALobbyDoor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>("BoxCollider");
	BoxCollider->SetCollisionProfileName("OverlapAll");
	BoxCollider->SetGenerateOverlapEvents(true);
	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &ALobbyDoor::OverlapBegin);
	BoxCollider->OnComponentEndOverlap.AddDynamic(this, &ALobbyDoor::OverlapEnd);
	SetRootComponent(BoxCollider);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>("DoorMesh");
	DoorMesh->SetCollisionProfileName("NoCollision");
	DoorMesh->SetupAttachment(RootComponent);

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ALobbyDoor::BeginPlay()
{
	Super::BeginPlay();

	BindInput();
}

// Called every frame
void ALobbyDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALobbyDoor::BindInput()
{
	// Initialize our component
	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();
	if (InputComponent)
	{
		InputComponent->BindAction<FInteractDelegate, ALobbyDoor, APawn*>("Interact", IE_Pressed, this, &ALobbyDoor::Interact, OverlappingCharacter);
	}
}

void ALobbyDoor::OverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ALobbyCharacter* OverlapPawn = Cast<ALobbyCharacter>(OtherActor);
	if (OverlapPawn && !OverlappingCharacter)
	{
		EnableInput(OverlapPawn->GetController<APlayerController>());
		OverlappingCharacter = OverlapPawn;
	}
}

void ALobbyDoor::OverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OverlappingCharacter)
		return;

	ALobbyCharacter* OverlapPawn = Cast<ALobbyCharacter>(OtherActor);
	if (OverlapPawn && OverlappingCharacter == OverlapPawn)
	{
		DisableInput(OverlapPawn->GetController<APlayerController>());
		OverlappingCharacter = nullptr;
	}
}

void ALobbyDoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyDoor, OverlappingCharacter);
}
