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
	// Niagara ������Ʈ ����
	LightningEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent1"));
	RootComponent = LightningEffect;
	LightningEffect2 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent2"));
	SetLightningComponent(LightningEffect);
	SetLightningComponent(LightningEffect2);
	
    
    CreateCollisionBox();
}
void AElectric::CreateCollisionBox()
{
    // �ݸ��� �ڽ� ���� �� ����
    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    //RootComponent = CollisionBox;  // ��Ʈ ������Ʈ�� ���� �Ǵ� SetupAttachment() ���
    CollisionBox->SetupAttachment(RootComponent);
    // �ݸ��� �ڽ� ũ�� ����
    CollisionBox->SetBoxExtent(FVector(50.0f, 50.0f, 50.0f*25));

    // �ݸ��� ������ ����
    CollisionBox->SetCollisionProfileName("OverlapOnlyPawn");

    // ����� ���� �ݸ��� ������ ���Ѵٸ�:
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // ����(������) ����
    CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);  // ��� ä�� ����
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);  // ���� ������

    // ������ �̺�Ʈ ���
    CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AElectric::OnOverlapBegin);
}
void AElectric::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    ARunner* Runner = Cast<ARunner>(OtherActor);
    if (Runner)
    {
        // ���⿡ ĳ������ �����Լ� ���� ����
        UE_LOG(LogTemp, Warning, TEXT("Pawn %s overlapped with %s"), *OtherActor->GetName(), *GetName());
        Runner->StunStart();
    }
}
void AElectric::SetLightningComponent(UNiagaraComponent* LightningActor)
{
	// ���̾ư��� ������Ʈ ����
	LightningActor->SetUsingAbsoluteRotation(false);
	LightningActor->SetUsingAbsoluteScale(false);
	LightningActor->SetUsingAbsoluteLocation(false);
	//���̼���	(vfx�� �߸𸣰ھ �ϵ��ڵ��ϴ� ����ۿ�����...)
	LightningActor->SetNiagaraVariableFloat(FString("Lightning Scale Vertical"), 2.2f);
	LightningActor->SetNiagaraVariableFloat(FString("Lightning Scale Horizontal"), 2.0f);
	//�����ü��� ����(���� ȭ������)
	LightningActor->SetNiagaraVariableFloat(FString("Lightning Lifetime Min"), 0.1f);
	LightningActor->SetNiagaraVariableFloat(FString("Lightning Lifetime Max"), 0.3f);
	//������ ����ȭ �õ�...
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
	MovingMaxVector = MaxVector-100.f;//100�� ���β� �����ϴ°���
	MovingMinVector = MinVector+100.f;

}

void AElectric::SetElectricMovingSpeed(const FVector& InputSpeed)
{
	MovingSpeed = InputSpeed;
}

void AElectric::SetElectricFlatten()
{
    bMoveHorizontally = false;
    // ���� ȸ���� ��������
    FRotator CurrentRotation = GetActorRotation();
    // yaw
    CurrentRotation.Pitch += 90.f;
    // ȸ�� ����
    SetActorRotation(CurrentRotation, ETeleportType::ResetPhysics);
    InitializeMovementDirection();//�ι� ����Ǿ(�����ɶ� �ѹ�, ���� �ѹ�) ȿ���� �������� �ϳ� ����� �ڵ带 ���� ��¿������
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
        // �̵� ���� ���� (�¿� �Ǵ� ����)
        if (bMoveHorizontally)
        {
            // �¿� �̵� (X��)
            MovementDirection = FVector(1.0f, -1.0f, 0.0f); // X�� ���, Y���� �׻� ����
        }
        else
        {
            // ���� �̵� (Z��)
            MovementDirection = FVector(0.0f, -1.0f, 1.0f); // Z�� ���, Y���� �׻� ����
        }
        if (FMath::RandBool())
        {
            MovementDirection *= -1;
        }
        // ���� ���ͷ� ����ȭ
        //MovementDirection.Normalize();

        // �ӵ� ���� ���
        MovingSpeed = MovementDirection * MovementSpeed;
    }
    else
    {
        MovementDirection= FVector(0.0f, -1.0f, 0.0f);
        MovingSpeed = MovementDirection * MovementSpeed;
    }
}
// ���� �̵� �Լ�
void AElectric::MoveActor(float DeltaTime)
{
    // ���� ��ġ
    FVector CurrentLocation = GetActorLocation();

    // �� ��ġ ���
    FVector NewLocation = CurrentLocation + (MovingSpeed * DeltaTime);

    // �浹 üũ �� ���� ����
    CheckBoundariesAndUpdateDirection(NewLocation);

    // Y ������ �׻� ������ �ǵ��� ����
    EnsureNegativeYDirection();

    // ��ġ ������Ʈ
    SetActorLocation(NewLocation);
}

// ��� üũ �� ���� ������Ʈ
void AElectric::CheckBoundariesAndUpdateDirection(FVector& NewLocation)
{
    // X�� ��� üũ
    if (NewLocation.X < MovingMinVector.X || NewLocation.X > MovingMaxVector.X)
    {
        // X ���� ����
        MovementDirection.X *= -1.0f;
        MovingSpeed.X *= -1.0f;
    }

    // Z�� ��� üũ
    if (NewLocation.Z < MovingMinVector.Z || NewLocation.Z > MovingMaxVector.Z)
    {
        // Z ���� ����
        MovementDirection.Z *= -1.0f;
        MovingSpeed.Z *= -1.0f;
    }

    // Y�� ��� üũ (�ʿ��� ���)
    if (NewLocation.Y < MovingMinVector.Y || NewLocation.Y > MovingMaxVector.Y)
    {
        // Y ���� ����
        MovementDirection.Y *= -1.0f;
        MovingSpeed.Y *= -1.0f;
    }
}

// Y ������ �׻� ������ �ǵ��� ����
void AElectric::EnsureNegativeYDirection()
{
    // Y ������ ������ ������ ����
    if (MovementDirection.Y > 0.0f)
    {
        MovementDirection.Y = -MovementDirection.Y;
        MovingSpeed.Y = -FMath::Abs(MovingSpeed.Y);
    }
}

// �̵� ������ ��ȯ�ϱ� ���� �ɼ� �Լ�
void AElectric::ToggleMovementDirection()
{
    // �̵� ���� ��ȯ (�¿� <-> ����)
    bMoveHorizontally = !bMoveHorizontally;

    // �̵� ���� ���ʱ�ȭ
    InitializeMovementDirection();
}
