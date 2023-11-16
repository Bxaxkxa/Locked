// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyInteractableBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ALobbyInteractableBase::ALobbyInteractableBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>("BoxCollider");
	BoxCollider->SetCollisionProfileName("BlockAll");
	SetRootComponent(BoxCollider);

	ActorMesh = CreateDefaultSubobject<UStaticMeshComponent>("ActorMesh");
	ActorMesh->SetCollisionProfileName("NoCollision");
	ActorMesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ALobbyInteractableBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ALobbyInteractableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALobbyInteractableBase::ShowInteractIndicator(bool bState)
{
	UActorComponent* Component = GetComponentByClass(UMeshComponent::StaticClass());

	if (!Component)
	{
		return;
	}

	UMeshComponent* MeshComponent = Cast<UMeshComponent>(Component);
	if (MeshComponent)
	{
		if (MeshComponent->bRenderCustomDepth == bState)
		{
			return;
		}

		MeshComponent->SetRenderCustomDepth(bState);
	}
}
