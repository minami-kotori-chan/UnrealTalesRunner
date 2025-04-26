// Fill out your copyright notice in the Description page of Project Settings.


#include "GhostTail.h"
#include "Components/PoseableMeshComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Components/PoseableMeshComponent.h"

// Sets default values
AGhostTail::AGhostTail()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PoseableMesh = CreateDefaultSubobject<UPoseableMeshComponent>(TEXT("POSEABLEMESH"));
	RootComponent = PoseableMesh;
	
	
}

// Called when the game starts or when spawned
void AGhostTail::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGhostTail::Init(USkeletalMeshComponent* Pawn)
{
	PoseableMesh->SetSkeletalMesh(SK_PoseMesh);

	PoseableMesh->CopyPoseFromSkeletalComponent(Pawn);
	TArray<UMaterialInterface*> Mats = PoseableMesh->GetMaterials();

	for (int i = 0; i < Mats.Num(); i++)
	{
		Materials.Add(UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(), GhostMaterial));
		PoseableMesh->SetMaterial(i, Materials[i]);
	}
	FadeCountDown = FadeOutTime;
	IsSpawned = true;
}



// Called every frame
void AGhostTail::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CurrentTime += DeltaTime;
	if (CurrentTime >= LifeTime)
	{
		Destroy();
	}
	
}
