// Fill out your copyright notice in the Description page of Project Settings.


#include "DungeonPath.h"
#include "Components/PointLightComponent.h"
#include "Trap.h"

// Sets default values
ADungeonPath::ADungeonPath()
{
	PrimaryActorTick.bCanEverTick = true;

	InitWallMesh();

}

void ADungeonPath::InitWallMesh()
{
	WallMesh.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorMeshComponent")));
	WallMesh.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftMeshComponent")));
	WallMesh.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightrMeshComponent")));
	WallMesh.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ForwardMeshComponent")));
	WallMesh.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackWardMeshComponent")));
	WallMesh.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CeilMeshComponent")));

	RootComponent = WallMesh[0];
	for (int i = 1; i < WallMesh.Num(); i++)
	{
		WallMesh[i]->SetupAttachment(RootComponent);
	}
	//WallMesh[1]->SetRelativeLocation(FVector(0.0f, 0.0f, 135.0f));
	//SkyLight = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLightComponent"));
	//SkyLight->SetupAttachment(RootComponent);
    //PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLightComponent"));
    //PointLightComponent->SetupAttachment(RootComponent);
    // 라이트 컴포넌트를 생성한 후 동적 그림자 끄기

}

// Called when the game starts or when spawned
void ADungeonPath::BeginPlay()
{
	Super::BeginPlay();
    
}
void ADungeonPath::SpawnTraps()
{
    // 액터의 바운드를 구합니다.
    FVector Origin, BoxExtent;

    BoxExtent=WallMesh[0]->Bounds.BoxExtent;
    Origin = WallMesh[0]->Bounds.Origin;
    // 바운드 최소/최대 좌표 계산 (가정: 액터가 월드 축에 맞게 정렬되어 있다고 가정)
    FVector Min = Origin - BoxExtent;
    // (여기서는 Min과 Origin의 Z값을 바닥 높이로 사용할 수 있습니다.)

    // Trap 액터의 크기 (사전에 정의된 값 또는 다른 방법으로 얻은 값)
    const float TrapWidth = 250.f; 
    const float TrapDepth = 250.f; 

    // 통로(바닥)의 전체 크기 (X, Y)
    float TotalWidth = BoxExtent.X * 2.0f;
    float TotalDepth = BoxExtent.Y * 2.0f;

    // 완전히 맞아떨어지는 셀의 개수 (남는 영역은 무시)
    int32 NumCellsX = FMath::FloorToInt(TotalWidth / TrapWidth);
    int32 NumCellsY = FMath::FloorToInt(TotalDepth / TrapDepth);

    // 배치 확률 (원하는 확률로 설정, 예: 50% 확률)
    const float SpawnProbability = 0.1f;

    // Trap 액터 클래스들을 변수로 보관 (블루프린트에서 설정하거나 코드에서 초기화)
    // 예: TrapActorTypeA, TrapActorTypeB
    for (int32 i = 0; i < NumCellsX; ++i)
    {
        for (int32 j = 0; j < NumCellsY; ++j)
        {
            // 각 셀의 중심 위치 계산
            float CellCenterX = Min.X + TrapWidth * (i + 0.5f);
            float CellCenterY = Min.Y + TrapDepth * (j + 0.5f);
            // 바닥에 배치하므로 Z는 바닥 높이(예: Origin.Z)로 설정
            FVector SpawnLocation(CellCenterX, CellCenterY, Origin.Z+BoxExtent.Z+1.f);

            // 랜덤으로 이 셀에 Trap을 배치할지 결정
            if (FMath::FRand() < SpawnProbability)
            {
                if (TrapActorAClass && TrapActorBClass)
                {
                    // 랜덤으로 두 트랩 중 하나 선택
                    //TSubclassOf<ATrap> ChosenTrapClass = FMath::RandBool() ? TrapActorA->GetClass() : TrapActorB->GetClass();
                    TSubclassOf<ATrap> ChosenTrapClass = FMath::RandBool() ? TrapActorAClass : TrapActorBClass;

                    // 원하는 위치와 회전값으로 스폰 (SpawnLocation은 계산된 위치)
                    FRotator SpawnRotation = FRotator::ZeroRotator;
                    GetWorld()->SpawnActor<ATrap>(ChosenTrapClass, SpawnLocation, SpawnRotation);
                }
                
            }
        }
    }
}
void ADungeonPath::ChangeWallMeshdisable(int32 MeshNumber)
{
    WallMesh[MeshNumber]->Deactivate();
    WallMesh[MeshNumber]->SetSimulatePhysics(false);
    WallMesh[MeshNumber]->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    WallMesh[MeshNumber]->SetVisibility(false, false);
}

void ADungeonPath::SetStartPath()
{
    ChangeWallMeshdisable(5);
    ChangeWallMeshdisable(3);
    ChangeWallMeshdisable(4);
    SpawnTraps();
}

void ADungeonPath::SetStartZPath()
{
    ChangeWallMeshdisable(0);
    SpawnTraps();
}

void ADungeonPath::SetZPath()
{
    ChangeWallMeshdisable(0);
    ChangeWallMeshdisable(5);
}

void ADungeonPath::SetYPath()
{
    ChangeWallMeshdisable(3);
    ChangeWallMeshdisable(4);
    SpawnTraps();
}

// Called every frame
void ADungeonPath::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

