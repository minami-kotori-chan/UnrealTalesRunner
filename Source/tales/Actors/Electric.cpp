// Fill out your copyright notice in the Description page of Project Settings.


#include "Electric.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "../SubSystem/CustomGameSystem.h"
#include "Components/BoxComponent.h"
#include "../Characters/Runner.h"
// Sets default values
AElectric::AElectric()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Niagara 컴포넌트 생성
	LightningEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent1"));
	RootComponent = LightningEffect;
	LightningEffect2 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent2"));
	SetLightningComponent(LightningEffect);
	SetLightningComponent(LightningEffect2);
	
    
    CreateCollisionBox();
}
void AElectric::CreateCollisionBox()
{
    // 콜리전 박스 생성 및 설정
    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    //RootComponent = CollisionBox;  // 루트 컴포넌트로 설정 또는 SetupAttachment() 사용
    CollisionBox->SetupAttachment(RootComponent);
    // 콜리전 박스 크기 설정
    CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f*25));

    // 콜리전 프로필 설정
    CollisionBox->SetCollisionProfileName("OverlapOnlyPawn");

    // 사용자 정의 콜리전 설정을 원한다면:
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 쿼리(오버랩) 전용
    CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);  // 모든 채널 무시
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);  // 폰만 오버랩

    // 오버랩 이벤트 등록
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AElectric::OnOverlapBegin);
}
void AElectric::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ARunner* Runner = Cast<ARunner>(OtherActor);
    if (Runner)
    {
        // 여기에 캐릭터의 스텀함수 콜을 넣자
        UE_LOG(LogTemp, Warning, TEXT("Pawn %s overlapped with %s"), *OtherActor->GetName(), *GetName());
        Runner->StunStart();
    }
}
void AElectric::SetLightningComponent(UNiagaraComponent* LightningActor)
{
	// 나이아가라 컴포넌트 설정
	LightningActor->SetUsingAbsoluteRotation(false);
	LightningActor->SetUsingAbsoluteScale(false);
	LightningActor->SetUsingAbsoluteLocation(false);
	//길이설정	(vfx를 잘모르겠어서 하드코딩하는 방법밖에없다...)
	LightningActor->SetNiagaraVariableFloat(FString("Lightning Scale Vertical"), 2.2f);
	LightningActor->SetNiagaraVariableFloat(FString("Lightning Scale Horizontal"), 2.0f);
	//라이팅수명 조절(좀더 화려해짐)
	LightningActor->SetNiagaraVariableFloat(FString("Lightning Lifetime Min"), 0.1f);
	LightningActor->SetNiagaraVariableFloat(FString("Lightning Lifetime Max"), 0.3f);
	//라이팅 최적화 시도...
	LightningActor->bSingleSampleShadowFromStationaryLights = true;
	LightningActor->bAffectDynamicIndirectLighting = false;

	LightningActor->SetupAttachment(RootComponent);
}

void AElectric::SetDataFromSubSystem()
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		UCustomGameSystem* CustomSubsystem = GameInstance->GetSubsystem<UCustomGameSystem>();
		if (CustomSubsystem)
		{
			InitBoundingBox(CustomSubsystem->DungeonStartVector, CustomSubsystem->DungeonEndVector);
		}
	}
}


void AElectric::InitBoundingBox(const FVector& MinVector, const FVector& MaxVector)
{
	MovingMaxVector = MaxVector-100.f;//100은 벽두께 보간하는것임
	MovingMinVector = MinVector+100.f;

}

void AElectric::SetElectricMovingSpeed(const FVector& InputSpeed)
{
	MovingSpeed = InputSpeed;
}

void AElectric::SetElectricFlatten()
{
    bMoveHorizontally = false;
    // 현재 회전값 가져오기
    FRotator CurrentRotation = GetActorRotation();
    // yaw
    CurrentRotation.Pitch += 90.f;
    // 회전 적용
    SetActorRotation(CurrentRotation, ETeleportType::ResetPhysics);
    InitializeMovementDirection();//두번 실행되어서(생성될때 한번, 지금 한번) 효율이 떨어지긴 하나 깔끔한 코드를 위해 어쩔수없음
}

// Called when the game starts or when spawned
void AElectric::BeginPlay()
{
	Super::BeginPlay();
    
    SetDataFromSubSystem();

    CanMove = FMath::RandBool();
    InitializeMovementDirection();
    
}



// Called every frame
void AElectric::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    MoveActor(DeltaTime);
}

void AElectric::InitializeMovementDirection()
{
    if(CanMove)
    {
        // 이동 방향 설정 (좌우 또는 상하)
        if (bMoveHorizontally)
        {
            // 좌우 이동 (X축)
            MovementDirection = FVector(1.0f, -1.0f, 0.0f); // X축 양수, Y축은 항상 음수
        }
        else
        {
            // 상하 이동 (Z축)
            MovementDirection = FVector(0.0f, -1.0f, 1.0f); // Z축 양수, Y축은 항상 음수
        }
        if (FMath::RandBool())
        {
            MovementDirection *= -1;
        }
        // 단위 벡터로 정규화
        //MovementDirection.Normalize();

        // 속도 벡터 계산
        MovingSpeed = MovementDirection * MovementSpeed;
    }
    else
    {
        MovementDirection= FVector(0.0f, -1.0f, 0.0f);
        MovingSpeed = MovementDirection * MovementSpeed;
    }
}
// 액터 이동 함수
void AElectric::MoveActor(float DeltaTime)
{
    // 현재 위치
    FVector CurrentLocation = GetActorLocation();

    // 새 위치 계산
    FVector NewLocation = CurrentLocation + (MovingSpeed * DeltaTime);

    // 충돌 체크 및 방향 반전
    CheckBoundariesAndUpdateDirection(NewLocation);

    // Y 방향이 항상 음수가 되도록 보장
    EnsureNegativeYDirection();

    // 위치 업데이트
    SetActorLocation(NewLocation);
}

// 경계 체크 및 방향 업데이트
void AElectric::CheckBoundariesAndUpdateDirection(FVector& NewLocation)
{
    // X축 경계 체크
    if (NewLocation.X < MovingMinVector.X || NewLocation.X > MovingMaxVector.X)
    {
        // X 방향 반전
        MovementDirection.X *= -1.0f;
        MovingSpeed.X *= -1.0f;
    }

    // Z축 경계 체크
    if (NewLocation.Z < MovingMinVector.Z || NewLocation.Z > MovingMaxVector.Z)
    {
        // Z 방향 반전
        MovementDirection.Z *= -1.0f;
        MovingSpeed.Z *= -1.0f;
    }

    // Y축 경계 체크 (필요한 경우)
    if (NewLocation.Y < MovingMinVector.Y || NewLocation.Y > MovingMaxVector.Y)
    {
        // Y 방향 반전
        MovementDirection.Y *= -1.0f;
        MovingSpeed.Y *= -1.0f;
    }
}

// Y 방향이 항상 음수가 되도록 보장
void AElectric::EnsureNegativeYDirection()
{
    // Y 방향이 양수라면 음수로 변경
    if (MovementDirection.Y > 0.0f)
    {
        MovementDirection.Y = -MovementDirection.Y;
        MovingSpeed.Y = -FMath::Abs(MovingSpeed.Y);
    }
}

// 이동 방향을 전환하기 위한 옵션 함수
void AElectric::ToggleMovementDirection()
{
    // 이동 방향 전환 (좌우 <-> 상하)
    bMoveHorizontally = !bMoveHorizontally;

    // 이동 방향 재초기화
    InitializeMovementDirection();
}
