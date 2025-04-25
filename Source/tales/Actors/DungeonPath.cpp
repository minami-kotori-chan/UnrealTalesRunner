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
    // ����Ʈ ������Ʈ�� ������ �� ���� �׸��� ����

}

// Called when the game starts or when spawned
void ADungeonPath::BeginPlay()
{
	Super::BeginPlay();
    
}
void ADungeonPath::SpawnTraps()
{
    // ������ �ٿ�带 ���մϴ�.
    FVector Origin, BoxExtent;

    BoxExtent=WallMesh[0]->Bounds.BoxExtent;
    Origin = WallMesh[0]->Bounds.Origin;
    // �ٿ�� �ּ�/�ִ� ��ǥ ��� (����: ���Ͱ� ���� �࿡ �°� ���ĵǾ� �ִٰ� ����)
    FVector Min = Origin - BoxExtent;
    // (���⼭�� Min�� Origin�� Z���� �ٴ� ���̷� ����� �� �ֽ��ϴ�.)

    // Trap ������ ũ�� (������ ���ǵ� �� �Ǵ� �ٸ� ������� ���� ��)
    const float TrapWidth = 250.f; 
    const float TrapDepth = 250.f; 

    // ���(�ٴ�)�� ��ü ũ�� (X, Y)
    float TotalWidth = BoxExtent.X * 2.0f;
    float TotalDepth = BoxExtent.Y * 2.0f;

    // ������ �¾ƶ������� ���� ���� (���� ������ ����)
    int32 NumCellsX = FMath::FloorToInt(TotalWidth / TrapWidth);
    int32 NumCellsY = FMath::FloorToInt(TotalDepth / TrapDepth);

    // ��ġ Ȯ�� (���ϴ� Ȯ���� ����, ��: 50% Ȯ��)
    const float SpawnProbability = 0.1f;

    // Trap ���� Ŭ�������� ������ ���� (�������Ʈ���� �����ϰų� �ڵ忡�� �ʱ�ȭ)
    // ��: TrapActorTypeA, TrapActorTypeB
    for (int32 i = 0; i < NumCellsX; ++i)
    {
        for (int32 j = 0; j < NumCellsY; ++j)
        {
            // �� ���� �߽� ��ġ ���
            float CellCenterX = Min.X + TrapWidth * (i + 0.5f);
            float CellCenterY = Min.Y + TrapDepth * (j + 0.5f);
            // �ٴڿ� ��ġ�ϹǷ� Z�� �ٴ� ����(��: Origin.Z)�� ����
            FVector SpawnLocation(CellCenterX, CellCenterY, Origin.Z+BoxExtent.Z+1.f);

            // �������� �� ���� Trap�� ��ġ���� ����
            if (FMath::FRand() < SpawnProbability)
            {
                if (TrapActorAClass && TrapActorBClass)
                {
                    // �������� �� Ʈ�� �� �ϳ� ����
                    //TSubclassOf<ATrap> ChosenTrapClass = FMath::RandBool() ? TrapActorA->GetClass() : TrapActorB->GetClass();
                    TSubclassOf<ATrap> ChosenTrapClass = FMath::RandBool() ? TrapActorAClass : TrapActorBClass;

                    // ���ϴ� ��ġ�� ȸ�������� ���� (SpawnLocation�� ���� ��ġ)
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

