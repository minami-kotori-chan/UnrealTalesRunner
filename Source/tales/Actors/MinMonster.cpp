// Fill out your copyright notice in the Description page of Project Settings.


#include "MinMonster.h"
#include "DungeonPath.h"
// Sets default values
AMinMonster::AMinMonster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;
	OnActorBeginOverlap.AddDynamic(this, &AMinMonster::OnMinMonsterOverlap);
}

// Called when the game starts or when spawned
void AMinMonster::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMinMonster::SetSpeed(float Fspeed)
{
	MoveSpeedExpend = Fspeed;
}

void AMinMonster::OnMinMonsterOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	UE_LOG(LogTemp, Warning, TEXT("MinMonster overlapped with: %s"), *OtherActor->GetName());
}

void AMinMonster::MoveActor(float DeltaTime)
{
	if (TotalDungeonLength == 0)
	{
		return;
	}
    FVector CurrentLocation = GetActorLocation();

    // Y ¡¬«•ø° TotalDungeonLength¿« 1/100 ∏∏≈≠ ¡ı∞°Ω√≈¥
    float NewY = CurrentLocation.Y + (TotalDungeonLength * DeltaTime / MoveSpeed)* MoveSpeedExpend;

	NewY = CurrentLocation.Y + MoveSpeedExpend * DeltaTime * MoveSpeed;

    FVector NewLocation = FVector(CurrentLocation.X, NewY, CurrentLocation.Z);
    SetActorLocation(NewLocation);
}

// Called every frame
void AMinMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveActor(DeltaTime);
}
