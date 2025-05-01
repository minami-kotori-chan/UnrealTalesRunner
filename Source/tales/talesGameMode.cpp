// Copyright Epic Games, Inc. All Rights Reserved.

#include "talesGameMode.h"
#include "talesCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Actors/DungeonPath.h"
#include "Characters/Runner.h"
#include <Kismet/GameplayStatics.h>
#include "Actors/MinMonster.h"
#include "Actors/Electric.h"
#include "Containers/Queue.h"
#include "SubSystem/CustomGameSystem.h"
#include "GameFramework/PlayerStart.h"

AtalesGameMode::AtalesGameMode()
{
	// set default pawn class to our Blueprinted character
    Runner = nullptr;
    PrimaryActorTick.bCanEverTick = true;
    CharacterProgress = 0.f;

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

    //GetWorldTimerManager().SetTimer(ElectricSpawnTimerHandle, this, &AtalesGameMode::SpawnElectricVFX, ElectricSpawnInterval, true);//Ÿ�̸� ����
    
}
void AtalesGameMode::SpawnTrigger()
{
    for (int32 i = 0; i < FMath::RandRange(1, 5) ; i++)
    {
        SpawnElectricVFX();
    }
}
void AtalesGameMode::SpawnElectricVFX()
{
    
    if (!SpawnedMaxMonster || !SpawnedMinMonster || DungeonPathArray.Num() == 0) return;


    // ���� ��ġ ����
    FVector SpawnLocation;
    bool RotateType = FMath::RandBool();
    SpawnElectricVFXSetting(DungeonPathArray[0]->GetComponentsBoundingBox(), SpawnLocation, RotateType);


    // ���� ��� ���� ����
    TSubclassOf<AElectric> SelectedElectricClass = (FMath::RandBool()) ? ElectricRed : ElectricBlue;

    if (SelectedElectricClass)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AElectric* SpawnedElectric = GetWorld()->SpawnActor<AElectric>(SelectedElectricClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
        if (SpawnedElectric)
        {
            
            if (RotateType)
            {
                SpawnedElectric->SetElectricFlatten();
            }
            
            // ť�� �߰�
            ElectricActorQueue.Enqueue(SpawnedElectric);
        }
    }
    
}
void AtalesGameMode::SpawnElectricVFXSetting(const FBox& BoundingBox,FVector& OutputVector,bool Type)
{
    OutputVector.Y = SpawnedMaxMonster->GetActorLocation().Y;

    if (Type)
    {
        OutputVector.X=BoundingBox.GetCenter().X;

        OutputVector.Z = FMath::RandRange(BoundingBox.Min.Z+100, BoundingBox.Max.Z-100);
    }
    else
    {
        OutputVector.Z = BoundingBox.GetCenter().Z;

        OutputVector.X = FMath::RandRange(BoundingBox.Min.X+100, BoundingBox.Max.X-100);
    }
}

void AtalesGameMode::BeginPlay()
{
	
    GetWorld()->GetTimerManager().SetTimer(ElectricSpawnTimerHandle, this, &AtalesGameMode::SpawnTrigger, ElectricSpawnInterval, true);
    //RestartAllPlayer();
    
    Super::BeginPlay();
    Runner = Cast<ARunner>(UGameplayStatics::GetPlayerCharacter(this, 0));
}
AActor* AtalesGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    // ��� PlayerStart ã��
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundActors);

    // ���ϴ� PlayerStart ã�� (��: Ư�� �±׸� ����)
    for (AActor* Actor : FoundActors)
    {
        APlayerStart* PlayerStart = Cast<APlayerStart>(Actor);
        if (PlayerStart && PlayerStart->PlayerStartTag == FName("Player1"))
        {
            UE_LOG(LogTemp, Warning, TEXT("FOUND PLAYERSTART"));
            return PlayerStart;
        }
    }
    UE_LOG(LogTemp, Warning, TEXT("NOT FOUND PLAYERSTART"));
    return Super::ChoosePlayerStart_Implementation(Player);
}
void AtalesGameMode::SetSubSystemData()
{
    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        UCustomGameSystem* CustomSubsystem = GameInstance->GetSubsystem<UCustomGameSystem>();
        if (CustomSubsystem)
        {
            // ����ý����� ��� �Լ��� ȣ���մϴ�.
            if (DungeonPathArray.Num())
            {
                CustomSubsystem->InitDungeonPathData(DungeonPathArray[0]);
            }
        }
    }
}

void AtalesGameMode::StartPlay()
{
    
    Super::StartPlay();
    //SpawnPath();
    //SpawnPathWithVertical();
    CalculateDungeonLength();

    SpawnAllMonster();
    SetSubSystemData();
}

void AtalesGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    
    Super::InitGame(MapName, Options, ErrorMessage);
    SpawnPathWithVertical();
}


void AtalesGameMode::SpawnPath()
{
    
    int32 SpawnCountTemp = 10;
    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        UCustomGameSystem* CustomSubsystem = GameInstance->GetSubsystem<UCustomGameSystem>();
        if (CustomSubsystem)
        {
            SpawnCountTemp = CustomSubsystem->DungeonLength;
        }
    }

    const int32 SpawnCount = SpawnCountTemp;
    UE_LOG(LogTemp, Warning, TEXT("DungeonPathLength is %d."),SpawnCount);

    FVector SpawnLocation(1000.0f, 0.0f, 0.0f);  // �ʱ� ��ġ
    const int32 UpwardCount = 5; // Z�� ����(����) ���� ����
    //Runner = Cast<ARunner>(UGameplayStatics::GetPlayerCharacter(this, 0));

    for (int32 i = 0; i < SpawnCount; i++)
    {
        if (!DungeonPathBP) // BP�� �Ҵ�Ǿ� ���� �ʴٸ� �������� ����
        {
            UE_LOG(LogTemp, Warning, TEXT("DungeonPathBP is not assigned."));
            return;
        }

        FActorSpawnParameters SpawnParams;
        FString ActorName = FString::Printf(TEXT("DungeonPath_%d"), i);
        SpawnParams.Name = FName(*ActorName);

        FTransform SpawnTransform;
        SpawnTransform.SetLocation(SpawnLocation);
        SpawnTransform.SetRotation(FQuat::Identity);

        // DungeonPathBP�� ����Ͽ� ���͸� �����մϴ�.
        ADungeonPath* NewDungeonPath = GetWorld()->SpawnActor<ADungeonPath>(DungeonPathBP, SpawnTransform, SpawnParams);
        if (NewDungeonPath)
        {
            DungeonPathArray.Add(NewDungeonPath);
            // ������ ��� ���ڸ� �����ͼ� Y��(Ȥ�� �����ϰ��� �ϴ� ����) ũ�⸦ ���
            FBox ActorBounds = NewDungeonPath->GetComponentsBoundingBox();
            FVector BoundsSize = ActorBounds.GetSize();

            // ���� ���, Y�� �������� �����ϰ� �ʹٸ�:
            float OffsetY = BoundsSize.Y;

            // ���� ������ ���� ��ġ�� ���� ������ ��ġ + offset
            SpawnLocation.Y += OffsetY;
        }
    }
}
void AtalesGameMode::SpawnPathWithVertical()
{
    const int32 HorizontalSpawnCount = GetSpawnCount();  // Y�� ���� ���� ����
    const int32 VerticalSpawnCount = 5;     // Z�� ����(����) ���� ����
    const int32 VerticalStartIndex = 4;
    FVector SpawnLocation(1000.0f, 0.0f, 0.0f);  // �ʱ� ��ġ

    //Runner = Cast<ARunner>(UGameplayStatics::GetPlayerCharacter(this, 0));

    if (!DungeonPathBP)
    {
        UE_LOG(LogTemp, Warning, TEXT("DungeonPathBP is not assigned."));
        return;
    }

    // ù ��° ��� ���� - ������
    FActorSpawnParameters SpawnParams;
    SpawnParams.Name = FName(TEXT("DungeonPath_Origin"));
    FTransform SpawnTransform;
    SpawnTransform.SetLocation(SpawnLocation);
    SpawnTransform.SetRotation(FQuat::Identity);

    ADungeonPath* FirstDungeonPath = GetWorld()->SpawnActor<ADungeonPath>(DungeonPathBP, SpawnTransform, SpawnParams);
    if (FirstDungeonPath)
    {
        DungeonPathArray.Add(FirstDungeonPath);
        FirstDungeonPath->SetYPath();
        // ���� �������� ��� ���� (���� �ױ�)
        
        // ù ��° ����� ���̸� ����ؼ� �� ������ ����
        FBox FirstActorBounds = FirstDungeonPath->GetComponentsBoundingBox();
        FVector FirstBoundsSize = FirstActorBounds.GetSize();

        // ���� �������� ��� ���� (Y���� ����)
        // ù ��° ��δ� �̹� ���������Ƿ� SpawnLocation���� Y�� �����¸� ���� �� ��° ��κ��� ����
        FVector HorizontalStartLocation = SpawnLocation;
        HorizontalStartLocation.Y += FirstBoundsSize.Y;

        // ���� ��θ� Y�� �������� ���� (ù ��°�� �̹� ���������Ƿ� Count - 1)
        SpawnHorizontalPaths(HorizontalStartLocation, HorizontalSpawnCount - 1, VerticalStartIndex);


        //���� �������� ����
        if (DungeonPathArray.Num()>VerticalStartIndex)
        {
            FVector VerticalStartLocation = DungeonPathArray[VerticalStartIndex]->GetActorLocation();
            VerticalStartLocation.Z += FirstBoundsSize.Z;
            SpawnVerticalPaths(VerticalStartLocation, VerticalSpawnCount);

            //DungeonPathArray[VerticalStartIndex]->SetStartPath();
        }
        
    }
}
int32 AtalesGameMode::GetSpawnCount()
{
    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        UCustomGameSystem* CustomSubsystem = GameInstance->GetSubsystem<UCustomGameSystem>();
        if (CustomSubsystem)
        {
            return CustomSubsystem->DungeonLength;
        }
    }
    return 10;
}
// Z������ ��θ� �״� �Լ�
void AtalesGameMode::SpawnVerticalPaths(const FVector& StartLocation, int32 Count)
{
    if (!DungeonPathBP)
    {
        UE_LOG(LogTemp, Warning, TEXT("DungeonPathBP is not assigned."));
        return;
    }

    FVector SpawnLocation = StartLocation;
    float OffsetZ = 0.0f;

    for (int32 i = 0; i < Count; i++)
    {
        // ù ��° ��δ� ���� ��ġ�� ����
        if (i > 0)
        {
            SpawnLocation.Z += OffsetZ;
        }

        FActorSpawnParameters SpawnParams;
        FString ActorName = FString::Printf(TEXT("DungeonPath_Vertical_%d"), i);
        SpawnParams.Name = FName(*ActorName);

        FTransform SpawnTransform;
        SpawnTransform.SetLocation(SpawnLocation);
        SpawnTransform.SetRotation(FQuat::Identity);

        ADungeonPath* NewDungeonPath = GetWorld()->SpawnActor<ADungeonPath>(DungeonPathBP, SpawnTransform, SpawnParams);
        if (NewDungeonPath)
        {
            DungeonPathZArray.Add(NewDungeonPath);
            NewDungeonPath->SetZPath();

            // ��� ���ڸ� �����ͼ� Z�� ũ�⸦ ���
            FBox ActorBounds = NewDungeonPath->GetComponentsBoundingBox();
            FVector BoundsSize = ActorBounds.GetSize();
            OffsetZ = BoundsSize.Z;
            if (i == Count - 1)
            {
                // �÷��̾� ��ŸƮ ���� ����
                FVector PlayerStartLocation = SpawnTransform.GetLocation();
                PlayerStartLocation.Z += 100.0f; // �ٴڿ��� �ణ ���� ��ġ

                APlayerStart* PlayerStart = GetWorld()->SpawnActor<APlayerStart>(APlayerStart::StaticClass(), PlayerStartLocation, FRotator::ZeroRotator);
                if (PlayerStart)
                {
                    PlayerStart->PlayerStartTag = FName("Player1");
                    UE_LOG(LogTemp, Warning, TEXT("player start spawn complete"));
                }
            }
        }
    }
}

// Y������ ��θ� �״� �Լ�
void AtalesGameMode::SpawnHorizontalPaths(const FVector& StartLocation, int32 Count, const int32& StartIndexNumber)
{
    if (!DungeonPathBP)
    {
        UE_LOG(LogTemp, Warning, TEXT("DungeonPathBP is not assigned."));
        return;
    }

    FVector SpawnLocation = StartLocation;
    float OffsetY = 0.0f;

    for (int32 i = 0; i < Count; i++)
    {
        // ù ��° ��δ� ���� ��ġ�� ����
        if (i > 0)
        {
            SpawnLocation.Y += OffsetY;
        }

        FActorSpawnParameters SpawnParams;
        FString ActorName = FString::Printf(TEXT("DungeonPath_Horizontal_%d"), i);
        SpawnParams.Name = FName(*ActorName);

        FTransform SpawnTransform;
        SpawnTransform.SetLocation(SpawnLocation);
        SpawnTransform.SetRotation(FQuat::Identity);

        ADungeonPath* NewDungeonPath = GetWorld()->SpawnActor<ADungeonPath>(DungeonPathBP, SpawnTransform, SpawnParams);
        if (NewDungeonPath)
        {
            DungeonPathArray.Add(NewDungeonPath);
            if (i == StartIndexNumber - 1)
            {
                NewDungeonPath->SetStartPath();
            }
            else
            {
                NewDungeonPath->SetYPath();
            }
            // ��� ���ڸ� �����ͼ� Y�� ũ�⸦ ���
            FBox ActorBounds = NewDungeonPath->GetComponentsBoundingBox();
            FVector BoundsSize = ActorBounds.GetSize();
            OffsetY = BoundsSize.Y;
        }
    }
}
void AtalesGameMode::SpawnAllMonster()
{
    SpawnMaxMonster();
    SpawnMinMonster();
}
void AtalesGameMode::SpawnMaxMonster()
{
    if (MaxMonster)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        FVector StartLocation = DungeonPathArray[0]->GetActorLocation();
        StartLocation.Y = TotalDungeonLength / 10;
        if (DungeonPathArray.Num() > 8)
        {
            StartLocation.Y= DungeonPathArray[7]->GetActorLocation().Y;
        }
        StartLocation.Z = DungeonPathArray[0]->GetComponentsBoundingBox().GetCenter().Z;
        SpawnedMaxMonster = GetWorld()->SpawnActor<AActor>(MinMonster, StartLocation, FRotator::ZeroRotator, SpawnParams);
        Cast<AMinMonster>(SpawnedMaxMonster)->SetPath(TotalDungeonLength);
        Cast<AMinMonster>(SpawnedMaxMonster)->SetSpeed(2.f);
    }
}
void AtalesGameMode::SpawnMinMonster()
{
    if (MinMonster)
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
        FVector StartLocation = DungeonPathArray[0]->GetActorLocation();
        StartLocation.Z = DungeonPathArray[0]->GetComponentsBoundingBox().GetCenter().Z;
        SpawnedMinMonster = GetWorld()->SpawnActor<AActor>(MinMonster, StartLocation, FRotator::ZeroRotator, SpawnParams);
        Cast<AMinMonster>(SpawnedMinMonster)->SetPath(TotalDungeonLength);
    }
}


void AtalesGameMode::CalculateDungeonLength()
{
    if (DungeonPathArray.Num() == 0)
    {
        TotalDungeonLength = 0.f;
        DungeonStartY = 0.f;
        return;
    }

    float MinY = FLT_MAX;
    float MaxY = -FLT_MAX;

    // �迭�� ����� ��� DungeonPath ������ Y�� ��谪�� ����մϴ�.
    for (ADungeonPath* DungeonPath : DungeonPathArray)
    {
        if (DungeonPath)
        {
            FBox Bounds = DungeonPath->GetComponentsBoundingBox();
            MinY = FMath::Min(MinY, Bounds.Min.Y);
            MaxY = FMath::Max(MaxY, Bounds.Max.Y);
        }
    }

    // ������ �� ���̴� �ִ� Y�� �ּ� Y�� ���̷� ���մϴ�.
    TotalDungeonLength = MaxY - MinY;
    // ������ ���� Y ��ǥ (��: ����� ��� �� ������)
    DungeonStartY = MinY;

    //UE_LOG(LogTemp, Log, TEXT("Total Dungeon Length: %f, Dungeon Start Y: %f"), TotalDungeonLength, DungeonStartY);
}

void AtalesGameMode::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateProgressBar();
    //UpdateProgressVal(DeltaTime);
    if (CharacterProgress <= MinimumProgress) 
    {
        if (GEngine)
        {
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("%f"), DeltaTime));
        }
    }
    UpdateElectricVFX(DeltaTime);
    
}

void AtalesGameMode::UpdateElectricVFX(float DeltaTime)
{
    if (!SpawnedMinMonster) return;

    FVector MinMonsterLocation = SpawnedMinMonster->GetActorLocation();

    // ť�� �պκ� ���͵� üũ
    while (!ElectricActorQueue.IsEmpty())
    {
        AElectric* ElectricActor;
        ElectricActorQueue.Peek(ElectricActor);

        if (!ElectricActor)
        {
            // ��ȿ���� ���� ���ʹ� ����
            ElectricActorQueue.Pop();
            continue;
        }

        // ���� ��ġ
        FVector NowLocation = ElectricActor->GetActorLocation();

        // �� ��ġ ���
        //float ElectricMoveSpeed = 1.f;
        //NewLocation.Y = NewLocation.Y - ElectricMoveSpeed * DeltaTime;
        //ElectricActor->SetActorLocation(NewLocation);

        // Y�� �������� Min ���͸� �������� Ȯ��
        if (GEngine)
        {
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("data: %f %f"), NowLocation.Y, MinMonsterLocation.Y));
            //UE_LOG(LogTemp, Warning, TEXT("data: %f %f"), NowLocation.Y, MinMonsterLocation.Y);
        }
        if (NowLocation.Y <= MinMonsterLocation.Y)
        {
            // Min ���͸� �������Ƿ� ����
            
            ElectricActorQueue.Pop();
            ElectricActor->Destroy();
        }
        else
        {
            // ���� �������� �ʾ����� �� �̻� üũ�� �ʿ� ����
            break;
        }
    }

    
}

void AtalesGameMode::UpdateProgressBar()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("DungeonLength: %f"), TotalDungeonLength));
    }
    if (!Runner || TotalDungeonLength <= 0.f)
    {
        return;
    }

    // �÷��̾��� ���� y ��ǥ�� ������
    float PlayerY = Runner->GetActorLocation().Y;
    float MinMonsterY = SpawnedMinMonster->GetActorLocation().Y;

    // �÷��̾��� ��ġ�� ������ ���۰� �� ���̷� ���� (�ʿ��� ���)
    float ClampedPlayerY = FMath::Clamp(PlayerY, DungeonStartY, DungeonStartY + TotalDungeonLength);
    float ClampMinMonsterY = FMath::Clamp(MinMonsterY, DungeonStartY, DungeonStartY + TotalDungeonLength);

    // ����� ���: (�÷��̾��� ��ġ - ������) / ��ü ����
    float Progress = (ClampedPlayerY - DungeonStartY) / TotalDungeonLength;
    MinimumProgress = (ClampMinMonsterY - DungeonStartY) / TotalDungeonLength;

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Progress: %f"), Progress));
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("MinProgress: %f"), MinimumProgress));
    }
    CharacterProgress = Progress;
    

    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        UCustomGameSystem* CustomSubsystem = GameInstance->GetSubsystem<UCustomGameSystem>();
        if (CustomSubsystem)
        {
            CustomSubsystem->OnProgressChanged.Broadcast(MinimumProgress, Progress);//��������Ʈ ȣ��
        }
    }

    //OnProgressUpdated.Broadcast(Progress, MinimumProgress);
}

void AtalesGameMode::UpdateProgressVal(float DeltaTime)
{
    //MinimumProgress += DeltaTime * MinSpeed;
    MaximumProgress += DeltaTime * MaxSpeed;
}

