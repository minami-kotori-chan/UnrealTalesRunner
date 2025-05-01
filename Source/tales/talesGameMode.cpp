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

    //GetWorldTimerManager().SetTimer(ElectricSpawnTimerHandle, this, &AtalesGameMode::SpawnElectricVFX, ElectricSpawnInterval, true);//타이머 생성
    
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


    // 생성 위치 설정
    FVector SpawnLocation;
    bool RotateType = FMath::RandBool();
    SpawnElectricVFXSetting(DungeonPathArray[0]->GetComponentsBoundingBox(), SpawnLocation, RotateType);


    // 레드 블루 랜덤 선택
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
            
            // 큐에 추가
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
    // 모든 PlayerStart 찾기
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), FoundActors);

    // 원하는 PlayerStart 찾기 (예: 특정 태그를 가진)
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
            // 서브시스템의 멤버 함수를 호출합니다.
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

    FVector SpawnLocation(1000.0f, 0.0f, 0.0f);  // 초기 위치
    const int32 UpwardCount = 5; // Z축 방향(위쪽) 생성 개수
    //Runner = Cast<ARunner>(UGameplayStatics::GetPlayerCharacter(this, 0));

    for (int32 i = 0; i < SpawnCount; i++)
    {
        if (!DungeonPathBP) // BP가 할당되어 있지 않다면 스폰하지 않음
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

        // DungeonPathBP를 사용하여 액터를 스폰합니다.
        ADungeonPath* NewDungeonPath = GetWorld()->SpawnActor<ADungeonPath>(DungeonPathBP, SpawnTransform, SpawnParams);
        if (NewDungeonPath)
        {
            DungeonPathArray.Add(NewDungeonPath);
            // 액터의 경계 상자를 가져와서 Y축(혹은 연결하고자 하는 방향) 크기를 계산
            FBox ActorBounds = NewDungeonPath->GetComponentsBoundingBox();
            FVector BoundsSize = ActorBounds.GetSize();

            // 예를 들어, Y축 방향으로 연결하고 싶다면:
            float OffsetY = BoundsSize.Y;

            // 다음 액터의 시작 위치는 현재 액터의 위치 + offset
            SpawnLocation.Y += OffsetY;
        }
    }
}
void AtalesGameMode::SpawnPathWithVertical()
{
    const int32 HorizontalSpawnCount = GetSpawnCount();  // Y축 방향 생성 개수
    const int32 VerticalSpawnCount = 5;     // Z축 방향(위쪽) 생성 개수
    const int32 VerticalStartIndex = 4;
    FVector SpawnLocation(1000.0f, 0.0f, 0.0f);  // 초기 위치

    //Runner = Cast<ARunner>(UGameplayStatics::GetPlayerCharacter(this, 0));

    if (!DungeonPathBP)
    {
        UE_LOG(LogTemp, Warning, TEXT("DungeonPathBP is not assigned."));
        return;
    }

    // 첫 번째 통로 생성 - 시작점
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
        // 수직 방향으로 통로 생성 (위로 쌓기)
        
        // 첫 번째 통로의 높이를 계산해서 그 위부터 시작
        FBox FirstActorBounds = FirstDungeonPath->GetComponentsBoundingBox();
        FVector FirstBoundsSize = FirstActorBounds.GetSize();

        // 수평 방향으로 통로 생성 (Y축을 따라)
        // 첫 번째 통로는 이미 생성했으므로 SpawnLocation에서 Y축 오프셋만 더해 두 번째 통로부터 시작
        FVector HorizontalStartLocation = SpawnLocation;
        HorizontalStartLocation.Y += FirstBoundsSize.Y;

        // 남은 통로를 Y축 방향으로 생성 (첫 번째는 이미 생성했으므로 Count - 1)
        SpawnHorizontalPaths(HorizontalStartLocation, HorizontalSpawnCount - 1, VerticalStartIndex);


        //수직 방향으로 생성
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
// Z축으로 통로를 쌓는 함수
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
        // 첫 번째 통로는 시작 위치에 생성
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

            // 경계 상자를 가져와서 Z축 크기를 계산
            FBox ActorBounds = NewDungeonPath->GetComponentsBoundingBox();
            FVector BoundsSize = ActorBounds.GetSize();
            OffsetZ = BoundsSize.Z;
            if (i == Count - 1)
            {
                // 플레이어 스타트 액터 스폰
                FVector PlayerStartLocation = SpawnTransform.GetLocation();
                PlayerStartLocation.Z += 100.0f; // 바닥에서 약간 위로 배치

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

// Y축으로 통로를 쌓는 함수
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
        // 첫 번째 통로는 시작 위치에 생성
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
            // 경계 상자를 가져와서 Y축 크기를 계산
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

    // 배열에 저장된 모든 DungeonPath 액터의 Y축 경계값을 계산합니다.
    for (ADungeonPath* DungeonPath : DungeonPathArray)
    {
        if (DungeonPath)
        {
            FBox Bounds = DungeonPath->GetComponentsBoundingBox();
            MinY = FMath::Min(MinY, Bounds.Min.Y);
            MaxY = FMath::Max(MaxY, Bounds.Max.Y);
        }
    }

    // 던전의 총 길이는 최대 Y와 최소 Y의 차이로 구합니다.
    TotalDungeonLength = MaxY - MinY;
    // 던전의 시작 Y 좌표 (예: 진행률 계산 시 기준점)
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

    // 큐의 앞부분 액터들 체크
    while (!ElectricActorQueue.IsEmpty())
    {
        AElectric* ElectricActor;
        ElectricActorQueue.Peek(ElectricActor);

        if (!ElectricActor)
        {
            // 유효하지 않은 액터는 제거
            ElectricActorQueue.Pop();
            continue;
        }

        // 현재 위치
        FVector NowLocation = ElectricActor->GetActorLocation();

        // 새 위치 계산
        //float ElectricMoveSpeed = 1.f;
        //NewLocation.Y = NewLocation.Y - ElectricMoveSpeed * DeltaTime;
        //ElectricActor->SetActorLocation(NewLocation);

        // Y축 기준으로 Min 몬스터를 지났는지 확인
        if (GEngine)
        {
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("data: %f %f"), NowLocation.Y, MinMonsterLocation.Y));
            //UE_LOG(LogTemp, Warning, TEXT("data: %f %f"), NowLocation.Y, MinMonsterLocation.Y);
        }
        if (NowLocation.Y <= MinMonsterLocation.Y)
        {
            // Min 몬스터를 지났으므로 제거
            
            ElectricActorQueue.Pop();
            ElectricActor->Destroy();
        }
        else
        {
            // 아직 도달하지 않았으면 더 이상 체크할 필요 없음
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

    // 플레이어의 현재 y 좌표를 가져옴
    float PlayerY = Runner->GetActorLocation().Y;
    float MinMonsterY = SpawnedMinMonster->GetActorLocation().Y;

    // 플레이어의 위치를 던전의 시작과 끝 사이로 제한 (필요한 경우)
    float ClampedPlayerY = FMath::Clamp(PlayerY, DungeonStartY, DungeonStartY + TotalDungeonLength);
    float ClampMinMonsterY = FMath::Clamp(MinMonsterY, DungeonStartY, DungeonStartY + TotalDungeonLength);

    // 진행률 계산: (플레이어의 위치 - 시작점) / 전체 길이
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
            CustomSubsystem->OnProgressChanged.Broadcast(MinimumProgress, Progress);//델리게이트 호출
        }
    }

    //OnProgressUpdated.Broadcast(Progress, MinimumProgress);
}

void AtalesGameMode::UpdateProgressVal(float DeltaTime)
{
    //MinimumProgress += DeltaTime * MinSpeed;
    MaximumProgress += DeltaTime * MaxSpeed;
}

