// Fill out your copyright notice in the Description page of Project Settings.


#include "MinMonster.h"
#include "DungeonPath.h"
#include "Camera/CameraShakeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "../Characters/Runner.h"

// Sets default values
AMinMonster::AMinMonster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;

    //OnActorBeginOverlap.AddDynamic(this, &AMinMonster::OnMinMonsterOverlap);


    CollisionBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBoxComponent"));
    CollisionBoxComponent->SetupAttachment(RootComponent);
    // ����� ���� �ݸ��� ������ ���Ѵٸ�:
    CollisionBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // ����(������) ����
    CollisionBoxComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CollisionBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);  // ��� ä�� ����
    CollisionBoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);  // ���� ������

    StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // ����(������) ����
    StaticMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);  // ��� ä�� ����
    StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);  // ���� ������

    // ������ �̺�Ʈ ���
    CollisionBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AMinMonster::OnCameraOverlapBegin);
    CollisionBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AMinMonster::OnCameraOverlapEnd);
    StaticMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AMinMonster::OnMinMonsterOverlap);
}
void AMinMonster::OnCameraOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UE_LOG(LogTemp, Warning, TEXT("Component overlapped with: %s"), *OtherActor->GetName());
    EnableShake = true;
    //PlayCameraShake();
}
void AMinMonster::OnCameraOverlapEnd(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex)
{
    UE_LOG(LogTemp, Warning, TEXT("Component overlapped Ended with: %s"), *OtherActor->GetName());
    EnableShake = false;
    PlayCameraShake();
}
// Called when the game starts or when spawned
void AMinMonster::BeginPlay()
{
	Super::BeginPlay();
    
}

void AMinMonster::PlayCameraShake()
{

    // ���� ���� �ó� �ش� �̺�Ʈ �߻� ��
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->ClientStopCameraShake(
            ShakeClass,           // ������ ī�޶� ����ũ Ŭ����
            true                 // true�� �����ϸ� ��� �����˴ϴ�
        );
        UGameplayStatics::PlayWorldCameraShake(
            GetWorld(),           // ���� ���ؽ�Ʈ
            ShakeClass,           // ī�޶� ����ũ Ŭ����
            GetActorLocation(),   // ���� �߽���
            0.f,               // ���� �ݰ�
            5000.f,              // �ܺ� �ݰ�
            1.0f,                 // ���� ����
            true                  // �Ÿ� ��� ���� ���� ����
        );
    }
}

void AMinMonster::SetSpeed(float Fspeed)
{
	MoveSpeedExpend = Fspeed;
}

void AMinMonster::OnMinMonsterOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("MinMonster overlapped with: %s"), *OtherActor->GetName());
    
    ARunner* Runner = Cast<ARunner>(OtherActor);
    if (Runner)
    {
        Runner->CharacterOut();
    }
}

void AMinMonster::MoveActor(float DeltaTime)
{
	if (TotalDungeonLength == 0)
	{
		return;
	}
    FVector CurrentLocation = GetActorLocation();
    if (GEngine)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("LOCATION: %f"), CurrentLocation.Y));
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("MinProgress: %f"), MinimumProgress));
    }
    if (CurrentLocation.Y >= MaxEnableY)
    {
        return;
    }

    // Y ��ǥ�� TotalDungeonLength�� 1/100 ��ŭ ������Ŵ
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
    CallCameraShake(DeltaTime);
}

void AMinMonster::CallCameraShake(float DeltaTime)
{
    if (!EnableShake) return;

    CurrentTime += DeltaTime;

    if (CurrentTime < RecallTime) return;

    CurrentTime = 0;

    PlayCameraShake();
}
