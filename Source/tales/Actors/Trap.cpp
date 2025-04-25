// Fill out your copyright notice in the Description page of Project Settings.


#include "Trap.h"
#include "../Characters/Runner.h"
#include "../Characters/CharacterState.h"
#include "Components/BoxComponent.h"

// Sets default values
ATrap::ATrap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TrapComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TrapMeshComponent"));
	RootComponent = TrapComponent;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBoxComponent"));
	CollisionBox->SetupAttachment(RootComponent);

	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	CollisionBox->SetGenerateOverlapEvents(true);

	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapBegin);
}

ATrap::ATrap(bool Type)
{
	this->Type = Type;
	ATrap();
}

// Called when the game starts or when spawned
void ATrap::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATrap::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("OverlapBegin"));
	}
	ARunner* OtherCharacter = Cast<ARunner>(OtherActor);
	if (OtherCharacter && !OtherCharacter->IsAnger() && OtherCharacter->CanMove())
	{
		
		OtherCharacter->StartBounce(GetActorLocation(),LaunchStrength,ZSize);

		if (Type)
		{
			Destroy();
		}
	}
}

// Called every frame
void ATrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

