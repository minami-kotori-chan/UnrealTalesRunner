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
    // 사용자 정의 콜리전 설정을 원한다면:
    CollisionBoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 쿼리(오버랩) 전용
    CollisionBoxComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CollisionBoxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);  // 모든 채널 무시
    CollisionBoxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);  // 폰만 오버랩

    StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 쿼리(오버랩) 전용
    StaticMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);  // 모든 채널 무시
    StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);  // 폰만 오버랩

    // 오버랩 이벤트 등록
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

    // 게임 시작 시나 해당 이벤트 발생 시
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        PlayerController->ClientStopCameraShake(
            ShakeClass,           // 중지할 카메라 쉐이크 클래스
            true                 // true로 설정하면 즉시 중지됩니다
        );
        UGameplayStatics::PlayWorldCameraShake(
            GetWorld(),           // 월드 컨텍스트
            ShakeClass,           // 카메라 셰이크 클래스
            GetActorLocation(),   // 진동 중심점
            0.f,               // 내부 반경
            5000.f,              // 외부 반경
            1.0f,                 // 감쇠 비율
            true                  // 거리 기반 감쇠 적용 여부
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

    // Y 좌표에 TotalDungeonLength의 1/100 만큼 증가시킴
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
